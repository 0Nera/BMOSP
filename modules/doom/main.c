#define DOOM_IMPLEMENTATION
#include "PureDOOM.h"
#include <system.h>

static const char name[] = "[APP]DOOM";
static const char message[] = "DOOM :)";

static void *doom_file_buffer = NULL;
static uint64_t doom_file_size = 0;
static uint64_t doom_file_seek = 0;
static uint32_t *doom_frame = NULL;
static int nsek = 100000000;

static void d_printf(const char *str) {
	log_printf("[DOOM] %s\n", str);
}

static void *d_malloc(int size) {
	void *addr = alloc(size);
	// log_printf("[DOOM]<malloc> %u - 0x%x\n", size, (uint64_t)addr);
	return addr;
}

static void d_free(void *addr) {
	// log_printf("[DOOM]<free> 0x%x\n", addr);
	free(addr);
}

static void d_gettime_fn(int *sec, int *usec) {
	*sec = get_time( ).second;
	//*usec = arch_get_tick( ) / 1000;
	*usec = nsek;
	nsek += 100000000;
}

static void d_exit_fn(int code) {
	log_printf("[DOOM]<exit> %d\n", code);
	delete_thread( );
	for (;;) { asm volatile("hlt"); }
}

static char *d_getenv_fn(const char *var) {
	// log_printf("[DOOM]<getenv> %s\n", var);
	return ".";
}

static void *d_open_fn(const char *filename, const char *mode) {
	// log_printf("[DOOM]<open> %s - %s\n", filename, mode);

	if (strcmp(filename, "@doom1.wad") == 0) {
		return (void *)12; // Возвращаем произвольное значение для "handle"
	}

	if (strcmp(filename, "./doom1.wad") == 0) {
		return (void *)12; // Возвращаем произвольное значение для "handle"
	}
	return NULL;
}

static int d_write_fn(void *handle, const void *buf, int count) {
	// log_printf("[DOOM]<write> %u\n", (unsigned int)handle);
	if (handle == (void *)12) {
		// Здесь можно записывать данные из buf в doom_file_buffer
		// memcpy(doom_file_buffer, buf, count);
		return count;
	}
	return 0;
}

static void d_close_fn(void *handle) {
	// log_printf("[DOOM]<close> %u\n", (unsigned int)handle);
	doom_file_seek = 0;
}

static int d_read_fn(void *handle, void *buf, int count) {
	// log_printf("[DOOM]<read> %u\n", (unsigned int)handle);
	if (handle == (void *)12) {
		// Проверяем, чтобы не попытаться прочитать больше данных, чем доступно
		if (doom_file_seek + count > doom_file_size) { count = (int)(doom_file_size - doom_file_seek); }
		memcpy(buf, (uint8_t *)doom_file_buffer + doom_file_seek, count);
		doom_file_seek += count;
		return count;
	}
	return 0;
}

static int d_seek_fn(void *handle, int offset, doom_seek_t origin) {
	// log_printf("[DOOM]<seek> %u\n", (unsigned int)handle);
	if (handle == (void *)12) {
		switch (origin) {
			case DOOM_SEEK_SET: doom_file_seek = offset; break;
			case DOOM_SEEK_CUR: doom_file_seek += offset; break;
			case DOOM_SEEK_END: doom_file_seek = doom_file_size + offset; break;
		}
		// Ограничиваем позицию указателя чтения/записи
		if (doom_file_seek > doom_file_size) { doom_file_seek = doom_file_size; }
		if (doom_file_seek < 0) { doom_file_seek = 0; }
		return 0;
	}
	return -1;
}

static int d_tell_fn(void *handle) {
	// log_printf("[DOOM]<tell> %u\n", (unsigned int)handle);
	if (handle == (void *)12) { return (int)doom_file_seek; }
	return -1;
}

static int d_eof_fn(void *handle) {
	// log_printf("[DOOM]<eof> %u\n", (unsigned int)handle);
	if (handle == (void *)12) { return doom_file_seek >= doom_file_size ? 1 : 0; }
	return -1;
}

static void app_main( ) {
	framebuffer_t f = alloc_framebuffer( );
	log_printf("%x %ux%u %u\n", f.address, f.width, f.height, f.bpp);
	char *argv[] = { "doom", "@doom1.wad" };
	doom_set_print(d_printf);
	doom_set_malloc(d_malloc, d_free);
	doom_set_file_io(d_open_fn, d_close_fn, d_read_fn, d_write_fn, d_seek_fn, d_tell_fn, d_eof_fn);
	doom_set_gettime(d_gettime_fn);
	doom_set_getenv(d_getenv_fn);
	doom_set_exit(d_exit_fn);
	doom_init(2, argv, 0);
	doom_update( );
	doom_frame = (uint32_t *)doom_get_framebuffer(4 /* RGBA */);
	for (;;) {
		doom_update( );
		doom_frame = (uint32_t *)doom_get_framebuffer(4 /* RGBA */);
		task_switch( );
	}
}

void __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);
	framebuffer_t f = alloc_framebuffer( );
	log_printf("%x %ux%u %u\n", f.address, f.width, f.height, f.bpp);
	env->ret = &((module_info_t){ .name = (char *)&name,
	                              .message = (char *)&message,
	                              .type = 0,
	                              .data_size = 0,
	                              .data = (void *)&app_main,
	                              .err_code = 0,
	                              .module_id = 0,
	                              .irq = 0,
	                              .irq_handler = 0,
	                              .get_func = 0,
	                              .after_init = 0 });
	mod_update_info(env);
	delete_thread( );
}
