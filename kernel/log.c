/**
 * log.c
 * Функции отладки
 *
 * Функционал отладки
 *
 */

#include <arch.h>
#include <fb.h>
#include <lock.h>
#include <log.h>
#include <mem.h>
#include <stdint.h>
#include <tool.h>

static char start_buffer[1024];
static char *log_buffer;
static uint64_t fb_pos_x = 4;
static uint64_t fb_pos_y = 0;
static uint64_t buf_pos = 0;
static uint64_t buf_max = 1024;
static lock_t log_lock = LOCK_INIT;

#define FONT_WIDTH 6 + 1
#define FONT_HEIGHT 8 + 1

void redraw_screen( );

// Получение кода символа в таблице
static inline uint32_t analyze(char glyth) {
	return ((uint8_t)glyth - 32) * 8;
}

// Вывод символа по координатам
static void print_char(uint64_t x, uint64_t y, char glyth) {
	uint32_t glyth_index = analyze(glyth);
	for (uint64_t i = 0; i < FONT_6X8_SLIM_CHAR_HEIGHT; i++) {
		fb_print_bits(x, y + i, font_6x8_slim[glyth_index + i]);
	}
}

void log_dump_buffer( ) {
	for (uint64_t i = 0; i < buf_pos; i++) { com_write_byte(log_buffer[i]); }
}

static void log_fb_putchar(char c) {
	if (c == '\0' || fb_init_status < 1) { return; }

	if (c == '\r') {
		log_buffer[--buf_pos] = 0;
	} else if (c == '\t') {
		fb_pos_x += FONT_6X8_SLIM_CHAR_WIDTH * 4;
	} else if (c == '\n') {
		fb_pos_x = 4;
		fb_pos_y += FONT_6X8_SLIM_CHAR_HEIGHT + 1;
	} else {
		if (fb_pos_x >= SCREEN_WIDTH) {
			fb_pos_x = 4;
			fb_pos_y += FONT_6X8_SLIM_CHAR_HEIGHT + 1;
		}

		if (fb_pos_y + FONT_6X8_SLIM_CHAR_HEIGHT >= SCREEN_HEIGHT) {
			// Дошли до нижнего края экрана
			while (log_buffer[0] != '\n') { tool_memmove(log_buffer, log_buffer + 1, --buf_pos); }
			tool_memmove(log_buffer, log_buffer + 1, --buf_pos);
			redraw_screen( );
			return;
		}

		print_char(fb_pos_x, fb_pos_y, c);
		fb_pos_x += FONT_6X8_SLIM_CHAR_WIDTH;
	}
}

void redraw_screen( ) {
	// Перерисовка экрана
	hal_memset_32(SCREEN_BUFFER, DARK_GREEN, SCREEN_WIDTH * SCREEN_HEIGHT);

	fb_pos_x = 4;
	fb_pos_y = 0;

	for (uint64_t i = 0; i < buf_pos; i++) { log_fb_putchar(log_buffer[i]); }
}

void log_putchar(char c) {
	log_buffer[buf_pos] = c;
	com_write_byte(c);

	if (buf_pos + 1 == buf_max) {
		// Смещение буфера на 1 символ влево
		for (uint64_t i = 0; i < buf_max - 1; i++) { log_buffer[i] = log_buffer[i + 1]; }

		if (fb_init_status < 1) { return; }
		redraw_screen( );
	} else {
		buf_pos++;

		if (fb_init_status < 1) { return; }
		log_fb_putchar(c);
	}
}

// Вывод текстового сообщения
void log_printf(char *str, ...) {
	LOCK(log_lock);
	va_list args;
	va_start(args, str);
	tool_format(&log_putchar, str, args);

	lock_release(log_lock);
	va_end(args);
}

void log_init_mem( ) {
	LOCK(log_lock);
	log_buffer = mem_alloc(((SCREEN_WIDTH - 4) / FONT_WIDTH) * (SCREEN_HEIGHT / FONT_HEIGHT));
	tool_memcpy(log_buffer, start_buffer, buf_max);
	buf_max = ((SCREEN_WIDTH - 4) / FONT_WIDTH) * (SCREEN_HEIGHT / FONT_HEIGHT);
	LOG("Размер буффера: %u символов\n", buf_max);
	LOG("%ux%u\n", width, height);
	redraw_screen( );
	lock_release(log_lock);
}

void log_init( ) {
	log_buffer = (char *)&start_buffer;
}