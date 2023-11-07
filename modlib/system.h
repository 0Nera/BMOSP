/**
 * system.h
 * Системные вызовы
 *
 * Заголовочный файл содержащий заготовку для работы с системными вызовами
 *
 */

#ifndef SYSTEM_H
#define SYSTEM_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;

typedef struct {
	int reserved;
} framebuffer_t;

typedef struct {
	int reserved;
} sys_info_t;

typedef struct {
	char *name;
	void *addr;
} module_func_t;

typedef struct {
	char *name;
	char *message;
	uint64_t type;
	uint64_t data_size;
	void *data;
	int64_t err_code;
	uint64_t module_id;
} module_info_t;

typedef struct {
	int reserved;
} func_t;

typedef struct {
	uint8_t a[4];
	uint8_t b[4];
	uint8_t c[4];
	uint8_t d[4];
} uid_t;

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t second;
} time_t;

typedef struct {
	uint64_t offset;
	module_info_t *info;
	void (*fb_printf)(char *str, ...); // Временная функция
	framebuffer_t *(*alloc_framebuffer)( );
	void (*free_framebuffer)(framebuffer_t *frame);
	void (*exit)(int code);
	void (*get_error)( );
	sys_info_t *(*get_info)( );
	module_info_t *(*get_module)(char *module_id);
	uint64_t (*new_thread)(uint64_t func);
	int (*delete_thread)(uint64_t thread_id);
	time_t (*get_time)( );
} env_t;

extern module_info_t static_info;
static void (*fb_printf)(char *str, ...);
static uint64_t offset;

static inline void init_env(env_t *loader_env) {
	offset = loader_env->offset;
	loader_env->info = (module_info_t *)&static_info + offset;
	fb_printf = loader_env->fb_printf;
}

#endif // system.h
