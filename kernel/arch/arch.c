/**
 * arch.c
 * Инициализация архитектуры
 *
 * Настройка архитектурнозависимых функций
 *
 */

#include <arch.h>

static uint64_t kernel_start_ticks = 0;
static uint64_t kernel_ticks = 0;

uint64_t rdtsc( ) {
	unsigned int lo, hi;
	asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)hi << 32) | lo;
}

uint64_t arch_get_tick( ) {
	if (kernel_start_ticks == 0) {
		kernel_start_ticks = rdtsc( );
		return 0;
	}

	kernel_ticks = rdtsc( ) - kernel_start_ticks;

	return kernel_ticks;
}

uint64_t arch_get_tick_b( ) {
	uint64_t tick = arch_get_tick( ) + 1;

	return tick / 1000000;
}

uint64_t arch_get_tick_l( ) {
	uint64_t tick = arch_get_tick( ) + 1;

	return tick % 1000000;
}

void arch_init( ) {
	gdt_init( );
	pic_init( );
	idt_init( );
	cpu_init( );
}