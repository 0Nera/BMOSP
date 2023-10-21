/**
 * sys.c
 * Файл с функциями управления системой
 *
 * Этот файл содержит имплементацию функций для управления системой
 */

#include <stdint.h>
#include <sys.h>

void sys_init( ) {}

framebuffer_t *sys_alloc_framebuffer( ) {
	return (framebuffer_t *)0;
}

void sys_free_framebuffer(framebuffer_t *frame) {}

void sys_exit(int code) {}

int sys_get_error( ) {
	return 0;
}

sys_info_t *sys_get_info( ) {}

module_info_t *sys_get_module(uid_t module_id) {}

uid_t sys_new_thread(func_t func) {}

int sys_delete_thread(uid_t thread_id) {
	return 0;
}

time_t sys_get_time( ) {}

void sys_set_alarm(time_t time, func_t func) {}
