#include "keymap.h"
#include <system.h>

static uint8_t current_state;
static bool kbd_free = false;
static int ru = 1;
static char c_char = '\0';
static key_event_t keyboard_buffer;

static void virt_exit( ) {
	outw(0x604, 0x2000);
	outw(0x4004, 0x3400);
	outw(0x600, 0x34);
}

static void wait_irq( ) {
	while (!kbd_free) { asm volatile("pause"); }
	kbd_free = false;
}

static char getkey( ) {
	wait_irq( );
	return c_char;
}

static char getc( ) {
__repeat:
	wait_irq( );
	if (c_char >= '0' && c_char <= '9') { return c_char; }

	if (c_char >= 'A' && c_char <= 'Z') { return c_char; }

	if (c_char >= 'a' && c_char <= 'z') { return c_char; }

	if (c_char >= 'А' && c_char <= 'Я') { return c_char; }

	if (c_char >= 'а' && c_char <= 'я') { return c_char; }

	goto __repeat;
}

static void *__get_func(uint64_t func) {
	switch (func) {
		case 0: return wait_irq;
		case 1: return getkey;
		case 2: return getc;
		default: return NULL;
	}
}

static void after_interrupt( ) {
	uint8_t status = inb(0x61) | 1;
	outb(0x61, status);
}

static int is_shift(uint8_t scancode) {
	switch (scancode) {
		case 0x2A: // Левый SHIFT
		case 0x36: // Правый SHIFT
			return 1;
		default: // Другой символ
			return 0;
	}
}

static int is_ctrl(uint8_t scancode) {
	switch (scancode) {
		case 0x1D: // Левый или правый CTRL
			return 1;
		default: // Другой символ
			return 0;
	}
}

static void handler( ) {
	while (!(inb(0x64) & 1)) { asm volatile("pause"); }

	uint8_t scancode = inb(0x60);
	char c = '\0';

	switch (scancode) {
		case 0x01:
			log_printf("ВЫХОД\n");
			virt_exit( );
			break; // Клавиша "ESCAPE"
		case 0x4F: // Клавиша "END"
			log_printf("END?\n");
			break;
		default: break;
	}

	if (scancode == 0xE0) {
		current_state = PREFIX_STATE;
		after_interrupt( );
		return;
	}

	if (scancode < 0) {
		after_interrupt( );
		return;
	}

	if (scancode == 0xAA || scancode == 0xB6) { // Отпущена клавиша SHIFT
		keyboard_buffer.shift_pressed = 0;
		after_interrupt( );
		return;
	}

	if (scancode == 0x9D) { // Отпущена клавиша CTRL
		keyboard_buffer.ctrl_pressed = 0;
		after_interrupt( );
		return;
	}

	if (is_shift(scancode)) { // Нажата клавиша SHIFT
		keyboard_buffer.shift_pressed = 1;
		after_interrupt( );
		return;
	}

	if (is_ctrl(scancode)) { // Нажата клавиша CTRL
		keyboard_buffer.ctrl_pressed = 1;
		after_interrupt( );
		return;
	}

	if (current_state == PREFIX_STATE) { current_state = NORMAL_STATE; }

	if (ru) {
		if (keyboard_buffer.shift_pressed && !keyboard_buffer.ctrl_pressed) {
			c = ru_chars_shifted[scancode];
		} else {
			c = ru_chars_lower[scancode];
		}
	} else {
		if (keyboard_buffer.shift_pressed && !keyboard_buffer.ctrl_pressed) {
			c = en_chars_shifted[scancode];
		} else {
			c = en_chars_lower[scancode];
		}
	}

	c_char = c;
	kbd_free = true;
	after_interrupt( );
}

void __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);
	current_state = NORMAL_STATE;
	keyboard_buffer.ctrl_pressed = 0;
	keyboard_buffer.shift_pressed = 0;

	env->ret = &((module_info_t){ .name = (char *)"[KEYBOARD]",
	                              .message = (char *)"PS/2 драйвер",
	                              .type = 0,
	                              .data_size = 0,
	                              .data = (void *)0,
	                              .err_code = 0,
	                              .module_id = 0,
	                              .irq = 33,
	                              .irq_handler = &handler,
	                              .get_func = __get_func });
	log_printf("Драйвер PS/2 клавиатуры установлен\n");
	set_int(33, handler);
	mod_update_info(env);
	delete_thread( );
}