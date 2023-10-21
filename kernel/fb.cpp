#include <6x8_slim_font.h>
#include <fb.h>
#include <limine.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <tool.h>

enum colors {
	WHITE = 0xFFFFFF,
	BLACK = 0x000000,
	RED = 0xFF0000,
	GREEN = 0x00FF00,
	BLUE = 0x0000FF,
	DARK_GREEN = 0x013220,
};

namespace fb {
static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0,
	.response = (struct limine_framebuffer_response *)0
};

struct limine_framebuffer_response *framebuffer_response;
struct limine_framebuffer *boot_framebuffer;

uint32_t *fb_addr;
uint32_t text_color = GREEN;
uint32_t background = DARK_GREEN;
uint64_t width;
uint64_t height;
uint64_t pitch;
uint16_t bpp;

size_t pos_x = 4;
size_t pos_y = 4;

#define SCREEN_WIDTH width
#define SCREEN_HEIGHT height
#define SCREEN_BUFFER fb_addr

void init( ) {
	framebuffer_response = framebuffer_request.response;
	boot_framebuffer = framebuffer_response->framebuffers[0];
	fb_addr = (uint32_t *)boot_framebuffer->address;
	width = boot_framebuffer->width;
	height = boot_framebuffer->height;
	bpp = boot_framebuffer->bpp;
	pitch = boot_framebuffer->pitch;

	for (uint64_t i = 0; i < width * height; i++) { fb_addr[i] = background; }

	fb::printf("0x%x %ux%u\n", fb_addr, width, height);
}

void print_buf(size_t x, size_t y, size_t h, size_t w, uint32_t *buf) {
	for (size_t j = 0; j < h; j++) {
		for (size_t i = 0; i < w; i++) {
			uint64_t where = (i + x) + (j + y) * width;
			SCREEN_BUFFER[where] = buf[i + j * w];
		}
	}
}

static inline void print_bits(size_t x, size_t y, uint8_t num) {
	for (size_t i = 0; i <= 7; i++) {
		if ((num >> i) & 1) {
			SCREEN_BUFFER[x + i + y * SCREEN_WIDTH] = text_color;
		}
	}
}

static inline uint32_t analyze(char glyth) {
	return ((uint8_t)glyth - 32) * 8;
}

static void print_char(int x, int y, char glyth) {
	for (size_t i = 0; i < FONT_6X8_SLIM_CHAR_HEIGHT; i++) {
		print_bits(x, y + i, font_6x8_slim[analyze(glyth) + i]);
	}
}

void scroll_fb( ) {
	size_t last_line_index =
	    (SCREEN_HEIGHT - (FONT_6X8_SLIM_CHAR_HEIGHT)) * SCREEN_WIDTH;

	for (size_t y = 0; y < SCREEN_HEIGHT - (FONT_6X8_SLIM_CHAR_HEIGHT); y++) {
		for (size_t x = 0; x < SCREEN_WIDTH; x++) {
			SCREEN_BUFFER[x + y * SCREEN_WIDTH] =
			    SCREEN_BUFFER[x +
			                  (y + (FONT_6X8_SLIM_CHAR_HEIGHT)) * SCREEN_WIDTH];
		}
	}

	for (size_t i = last_line_index; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
		SCREEN_BUFFER[i] = background;
	}
}

static void fb_putchar(char c) {
	if (c == '\t') {
		pos_x += FONT_6X8_SLIM_CHAR_WIDTH * 4;
	} else if (c == '\n') {
		// Новая строка
		pos_x = 4;
		pos_y += FONT_6X8_SLIM_CHAR_HEIGHT + 1;
	} else {
		if (pos_x >= SCREEN_WIDTH) {
			pos_x = 4;
			pos_y += FONT_6X8_SLIM_CHAR_HEIGHT + 1;
		}
		if (pos_y >= SCREEN_HEIGHT - FONT_6X8_SLIM_CHAR_HEIGHT) {
			scroll_fb( );
			pos_y -= FONT_6X8_SLIM_CHAR_HEIGHT;
		}
		print_char(pos_x, pos_y, c);
		pos_x += FONT_6X8_SLIM_CHAR_WIDTH;
	}
}

void printf(char *str, ...) {
	va_list args;
	va_start(args, str);
	tool::format(&fb_putchar, str, args);
	va_end(args);
}

void printf_at(size_t x, size_t y, char *str, ...) {
	va_list args;
	va_start(args, str);

	// Сохраняем текущие значения pos_x и pos_y
	size_t prev_pos_x = pos_x;
	size_t prev_pos_y = pos_y;

	// Устанавливаем новые значения координат вывода
	pos_x = x;
	pos_y = y;

	// Выводим строку
	tool::format(&fb_putchar, str, args);

	// Восстанавливаем предыдущие значения pos_x и pos_y
	pos_x = prev_pos_x;
	pos_y = prev_pos_y;

	va_end(args);
}
} // namespace fb