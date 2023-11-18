/**
 * sys.c
 * Файл с функциями управления системой
 *
 * Этот файл содержит имплементацию функций для управления системой
 */

#include <fb.h>
#include <mod.h>
#include <stdint.h>
#include <sys.h>

module_info_t *current_module;

void sys_init( ) {}

static framebuffer_t *sys_alloc_framebuffer( ) {
	return (framebuffer_t *)0;
}

static void sys_free_framebuffer(framebuffer_t *frame) {
	frame->reserved = 0;
}

static void sys_exit(int code) {
	current_module->err_code = code;
}

static int sys_get_error( ) {
	return current_module->err_code;
}

static sys_info_t *sys_get_info( ) {
	return &(sys_info_t){ .reserved = 0 };
}

static module_info_t *sys_get_module(char *module_id) {
	return (module_info_t *)mod_find(module_id);
}

static uint64_t sys_new_thread(uint64_t func) {
	return func;
}

static int sys_delete_thread(uint64_t thread_id) {
	return thread_id;
}

static time_t sys_get_time( ) {
	return (time_t){ .year = 2023, .month = 10, .day = 31, .second = 1 };
}

env_t *sys_install(env_t *module) {
	module->fb_printf = &fb_printf;
	module->alloc_framebuffer = &sys_alloc_framebuffer;
	module->free_framebuffer = &sys_free_framebuffer;
	module->exit = &sys_exit;
	module->get_error = &sys_get_error;
	module->get_info = &sys_get_info;
	module->get_module = &sys_get_module;
	module->new_thread = &sys_new_thread;
	module->delete_thread = &sys_delete_thread;
	module->get_time = &sys_get_time;

	return module;
}

// void sys_set_alarm(time_t time, func_t func) {}
