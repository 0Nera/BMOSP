/**
 * sys.c
 * Файл с функциями управления системой
 *
 * Этот файл содержит имплементацию функций для управления системой
 */

#include <stdint.h>
#include <sys.h>

module_info_t *current_module;

void sys_init( ) {}

framebuffer_t *sys_alloc_framebuffer( ) {
	return (framebuffer_t *)0;
}

void sys_free_framebuffer(framebuffer_t *frame) {
	frame->reserved = 0;
}

void sys_exit(int code) {
	current_module->err_code = code;
}

int sys_get_error( ) {
	return current_module->err_code;
}

sys_info_t *sys_get_info( ) {
	return &(sys_info_t){ .reserved = 0 };
}

module_info_t *sys_get_module(uint64_t module_id) {
	return (module_info_t *)module_id;
}

uint64_t sys_new_thread(uint64_t func) {
	return func;
}

int sys_delete_thread(uint64_t thread_id) {
	return thread_id;
}

time_t sys_get_time( ) {
	return (time_t){ .year = 2023, .month = 10, .day = 31, .second = 1 };
}

// void sys_set_alarm(time_t time, func_t func) {}
