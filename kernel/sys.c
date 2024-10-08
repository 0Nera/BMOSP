/**
 * sys.c
 * Файл с функциями управления системой
 *
 * Этот файл содержит имплементацию функций для управления системой
 */

#include <fb.h>
#include <log.h>
#include <mem.h>
#include <mod.h>
#include <stdint.h>
#include <sys.h>

module_info_t *current_module;
extern uint32_t *fb_addr;
extern uint32_t text_color;
extern uint32_t background;
extern uint64_t width;
extern uint64_t height;
extern uint64_t pitch;
extern uint16_t bpp;

void sys_init( ) {}

static framebuffer_t sys_alloc_framebuffer( ) {
	return (framebuffer_t){
		.address = fb_addr, .width = width, .height = height, .pitch = pitch, .bpp = bpp, .reserved = 0
	};
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

env_t *sys_install(env_t *module) {
	module->log_printf = &log_printf;
	module->alloc_framebuffer = &sys_alloc_framebuffer;
	module->free_framebuffer = &sys_free_framebuffer;
	module->alloc = &mem_alloc;
	module->free = &mem_free;
	module->exit = &sys_exit;
	module->get_error = &sys_get_error;
	module->get_info = &sys_get_info;
	module->get_module = &sys_get_module;
	module->mod_list_get = &mod_list_get;
	module->new_thread = &task_new_thread;
	module->delete_thread = &task_del_current;
	module->task_switch = &task_switch;
	module->get_time = &rtc_get_time;
	module->set_int = &idt_set_int;
	module->mod_update_info = &mod_update_info;
	module->ret = NULL;

	return module;
}

// void sys_set_alarm(time_t time, func_t func) {}
