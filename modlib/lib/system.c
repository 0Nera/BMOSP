/**
 * system.c
 * Системные вызовы модулей
 *
 * Основные функции для работы с функцияи системы
 *
 */

#include <modstd.h>
#include <system.h>

void *(*alloc)(uint64_t size);
void (*free)(void *ptr);
void (*fb_printf)(char *str, ...);
module_info_t *(*get_module)(char *module_id);
framebuffer_t (*alloc_framebuffer)( );
void (*free_framebuffer)(framebuffer_t *frame);
void (*exit)(int code);
int (*get_error)( );
sys_info_t *(*get_info)( );
uint64_t (*new_thread)(uint64_t func);
int (*delete_thread)(uint64_t thread_id);
time_t (*get_time)( );
uint64_t offset;

void init_env(env_t *loader_env) {
	offset = loader_env->offset;
	fb_printf = loader_env->fb_printf;
	alloc = loader_env->alloc;
	free = loader_env->free;
	get_module = loader_env->get_module;
	alloc_framebuffer = loader_env->alloc_framebuffer;
	free_framebuffer = loader_env->free_framebuffer;
	exit = loader_env->exit;
	get_error = loader_env->get_error;
	get_info = loader_env->get_info;
	new_thread = loader_env->new_thread;
	delete_thread = loader_env->delete_thread;
	get_time = loader_env->get_time;
}

void *realloc(void *addr, size_t size) {
	if (size == 0) {
		free(addr);
		return NULL;
	}

	if (addr == NULL) { return alloc(size); }

	void *new_addr = alloc(size);

	if (new_addr == NULL) { return NULL; }

	memcpy(new_addr, addr, size);
	free(addr);

	return new_addr;
}