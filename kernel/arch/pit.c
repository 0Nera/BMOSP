/**
 * pit.c
 * Программируемый интервальный таймер
 *
 * Настройка программируемого интервального таймера и
 *
 */

#include <arch.h>
#include <fb.h>
#include <stdbool.h>
#include <stdint.h>
#include <tool.h>

static volatile uint64_t count = 0;
static volatile uint64_t test_color = 0x00D000;

extern uint32_t width;

static void isr_local( ) {
	if (test_color >= 0xFFFFFF) { test_color = 0x00D000; }

	uint32_t last = fb_get_text_color( );

	fb_set_text_color(test_color);
	fb_printf_at(SCREEN_WIDTH - 6 * 7, 0, "БМПОС");
	fb_set_text_color(last);

	count++;
	test_color += 0x010101;
}

void pit_set_interval(int hz) {
	int divisor = 1193180 / hz; // Вычисляем делитель
	outb(0x43, 0x34);           // Устанавливаем байт команды 0x34
	outb(0x40, divisor & 0xFF); // Устанавливаем младший байт делителя
	outb(0x40, divisor >> 8);   // Устанавливаем старший байт делителя
}

void pit_init( ) {
	idt_set_int(32, isr_local);
	pit_set_interval(1);
}