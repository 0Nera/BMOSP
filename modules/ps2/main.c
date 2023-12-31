#include "keymap.h"
#include <system.h>

static uint8_t current_state;
static bool kbd_free = false;
static int ru = 1;
static char c_char = '\0';
static key_event_t keyboard_buffer;

static inline void virt_exit( ) {
	fb_printf("Выход для Bochs\n");
	outw(0xB004, 0x2000);

	fb_printf("Выход для Qemu\n");
	outw(0x604, 0x2000);

	fb_printf("Выход для Virtualbox\n");
	outw(0x4004, 0x3400);

	fb_printf("Выход для облачного гипервизора\n");
	outw(0x600, 0x34);
}

static void wait_irq( ) {
	while (!kbd_free) { asm volatile("pause"); }
	kbd_free = false;
}

static char getc( ) {
	wait_irq( );
	return c_char;
}

static void *__get_func(uint64_t func) {
	switch (func) {
		case 0: return wait_irq;
		case 1: return getc;
		default: return NULL;
	}
}

static uint8_t keyboard_to_ascii(uint8_t key) {}

static void after_interrupt( ) {
	uint8_t status = inb(0x61) | 1;
	outb(0x61, status);
}

static int is_shift(uint8_t scancode) {
	switch (scancode) {
		case 0x2A: // Левый SHIFT
			return 1;
		case 0x36: // Правый SHIFT
			return 1;
		case 0xAA: // Левый SHIFT отпущен
			return -1;
		case 0xB6: // Правый SHIFT отпущен
			return -1;
		default: // Другой символ
			return 0;
	}
}

static int is_ctrl(uint8_t scancode) {
	if (current_state == PREFIX_STATE) {
		switch (scancode) {
			case 0x1D: // Правый CTRL
				return 1;
			case 0x9D: // Правый CTRL отпущен
				return -1;
			default: // Другой символ
				return 0;
		}
	}
	switch (scancode) {
		case 0x1D: // Левый CTRL
			return 1;
		case 0x9D: // Левый CTRL отпущен
			return -1;
		default: // Другой символ
			return 0;
	}
}

static void handler( ) {
	while (!(inb(0x64) & 1)) { asm volatile("pause"); }

	uint8_t scancode = inb(0x60);
	char c = ' \0';

	if (scancode == 0xE0) {
		current_state = PREFIX_STATE;
		after_interrupt( );
		return;
	}

	if (scancode < 0) {
		after_interrupt( );
		return;
	}

	if (is_shift(scancode) != 0) {
		keyboard_buffer->shift_pressed = is_shift(scancode);
		after_interrupt( );
		return;
	}

	if (is_ctrl(scancode) != 0) {
		keyboard_buffer->ctrl_pressed = is_ctrl(scancode);
		after_interrupt( );
		return;
	}

	if (current_state == PREFIX_STATE) { current_state = NORMAL_STATE; }

	
	if (ru) {
		if (keyboard_buffer.shift_pressed) {
			c = ru_chars_shifted[scancode];
		} else {
			c = ru_chars_lower[scancode];
		}
	} else {
		if (keyboard_buffer.shift_pressed) {
			c = en_chars_shifted[scancode];
		} else {
			c = en_chars_lower[scancode];
		}
	}

	c_char = c;
	kbd_free = true;

	switch (scancode) {
		case 0x01: virt_exit( ); break; // Клавиша "ESCAPE"
		case 0x4F:                      // Клавиша "END"
			asm volatile("int $32");
			break;
		default: break;
	}
	after_interrupt( );
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);
	current_state = NORMAL_STATE;
	return (module_info_t){ .name = (char *)"[KEYBOARD]",
		                    .message = (char *)"PS/2 драйвер",
		                    .type = 0,
		                    .data_size = MAX_KEY_BUFFER_SIZE,
		                    .data = (void *)&keyboard_buffer,
		                    .err_code = 0,
		                    .module_id = 0,
		                    .irq = 33,
		                    .irq_handler = handler,
		                    .get_func = __get_func };
}