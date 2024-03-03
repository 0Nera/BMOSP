/**
 * fb.h
 * Заголовок с функциями фреймбуффера
 *
 * Данный заголовочный файл содержит определения которые используются для работы
 * с экранным буффером(фреймбуффером)
 *
 */

#ifndef FB_H
#define FB_H

#include <6x8_slim_font.h>
#include <arch.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <tool.h>

enum colors {
	WHITE = 0xFFFFFF,
	BLACK = 0x000000,
	RED = 0xFF0000,
	GREEN = 0x00D000,
	PRIMA_GREEN = 0x00FF00,
	BLUE = 0x0000FF,
	DARK_GREEN = 0x013220,
};

extern int fb_init_status;
extern uint32_t *fb_addr;
extern uint64_t width;
extern uint64_t height;

#define SCREEN_WIDTH width
#define SCREEN_HEIGHT height
#define SCREEN_BUFFER fb_addr

void fb_set_text_color(uint32_t color);
uint32_t fb_get_text_color( );
void fb_init( );
void fb_print_buf(size_t x, size_t y, size_t h, size_t w, uint32_t *buf);
void log_printf(char *str, ...);
void log_printf_at(size_t x, size_t y, char *str, ...);
void fb_print_bits(size_t x, size_t y, uint8_t num);

#endif // fb.h