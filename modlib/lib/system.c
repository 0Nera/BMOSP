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
void (*log_printf)(char *str, ...);
module_info_t *(*get_module)(char *module_id);
module_info_t *(*mod_list_get)(uint64_t *count);
framebuffer_t (*alloc_framebuffer)( );
void (*free_framebuffer)(framebuffer_t *frame);
void (*exit)(int code);
int (*get_error)( );
sys_info_t *(*get_info)( );
uint64_t (*new_thread)(void (*func)(void *), char *name, void *arg);
void (*delete_thread)( );
time_t (*get_time)( );
void (*mod_update_info)(env_t *ret);
void (*set_int)(uint8_t vector, void (*func)(void *));
uint64_t offset;

void init_env(env_t *loader_env) {
	if (loader_env == NULL) {
		for (;;) {}
	}
	offset = loader_env->offset;
	log_printf = loader_env->log_printf;
	alloc = loader_env->alloc;
	free = loader_env->free;
	get_module = loader_env->get_module;
	mod_list_get = loader_env->mod_list_get;
	alloc_framebuffer = loader_env->alloc_framebuffer;
	free_framebuffer = loader_env->free_framebuffer;
	exit = loader_env->exit;
	get_error = loader_env->get_error;
	get_info = loader_env->get_info;
	new_thread = loader_env->new_thread;
	delete_thread = loader_env->delete_thread;
	get_time = loader_env->get_time;
	mod_update_info = loader_env->mod_update_info;
	set_int = loader_env->set_int;
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