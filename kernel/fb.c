/**
 * fb.c
 * Функции управления фреймбуффером
 *
 * Функционал управления выводом на экранный буффер (фреймбуффер) текста и
 * изображений
 *
 */

#include <6x8_slim_font.h>
#include <fb.h>
#include <limine.h>
#include <log.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <tool.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = (struct limine_framebuffer_response *)0
};

static struct limine_framebuffer_response *framebuffer_response;
static struct limine_framebuffer *boot_framebuffer;

int fb_init_status = 0;
uint32_t *fb_addr;
uint32_t text_color = GREEN;
uint32_t background = DARK_GREEN;
uint64_t width;
uint64_t height;
uint64_t pitch;
uint16_t bpp;
uint64_t pos_x = 0;
uint64_t pos_y = 0;

// Настройка прослойки графики ядра
void fb_init( ) {
	framebuffer_response = framebuffer_request.response;

	if (framebuffer_response == NULL) { return; }

	if (framebuffer_response->framebuffer_count < 1) { return; }

	fb_init_status = framebuffer_response->framebuffer_count;

	boot_framebuffer = framebuffer_response->framebuffers[0];
	fb_addr = (uint32_t *)boot_framebuffer->address;
	width = boot_framebuffer->width;
	height = boot_framebuffer->height;
	bpp = boot_framebuffer->bpp;
	pitch = boot_framebuffer->pitch;

	for (uint64_t i = 0; i < width * height; i++) { fb_addr[i] = background; }

	if (framebuffer_response->framebuffer_count == 1) { return; }

#ifdef DEBUG_FB
	LOG("Инициализация дополнительных: %u мониторов\n", framebuffer_response->framebuffer_count);
#endif
	for (uint64_t i = 1; i < framebuffer_response->framebuffer_count; i++) {
		struct limine_framebuffer *framebuffer = framebuffer_response->framebuffers[i];
		uint32_t *framebuffer_addr = (uint32_t *)framebuffer->address;
#ifdef DEBUG_FB
		LOG("[%u]->0x%x %ux%u\n", i, framebuffer->address, framebuffer->width, framebuffer->height);
#endif
		for (uint64_t ij = 0; ij < width * height; ij++) { framebuffer_addr[ij] = background; }
	}
}

// Отрисовка буффера по координатам (полезно для картинок)
void fb_print_buf(uint64_t x, uint64_t y, uint64_t h, uint64_t w, uint32_t *buf) {
	for (uint64_t j = 0; j < h; j++) {
		for (uint64_t i = 0; i < w; i++) {
			uint64_t where = (i + x) + (j + y) * width;
			SCREEN_BUFFER[where] = buf[i + j * w];
		}
	}
}

void fb_print_bits(size_t x, size_t y, uint8_t num) {
	for (size_t i = 0; i <= 7; i++) {
		if ((num >> i) & 1) { SCREEN_BUFFER[x + i + y * SCREEN_WIDTH] = text_color; }
	}
}

void scroll_fb( ) {
	size_t last_line_index = (SCREEN_HEIGHT - (FONT_6X8_SLIM_CHAR_HEIGHT)) * SCREEN_WIDTH;

	for (uint64_t y = 0; y < SCREEN_HEIGHT - (FONT_6X8_SLIM_CHAR_HEIGHT); y++) {
		for (uint64_t x = 0; x < SCREEN_WIDTH; x++) {
			SCREEN_BUFFER[x + y * SCREEN_WIDTH] = SCREEN_BUFFER[x + (y + (FONT_6X8_SLIM_CHAR_HEIGHT)) * SCREEN_WIDTH];
		}
	}

	for (uint64_t i = last_line_index; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) { SCREEN_BUFFER[i] = background; }
}