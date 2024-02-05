/**
 * arch.c
 * Инициализация архитектуры
 *
 * Настройка архитектурнозависимых функций
 *
 */

#include <log.h>
#include <arch.h>

typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint64_t base;
} gdt_reg_t;

struct gdt_desc {
	uint16_t limit;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_hi;
} __attribute__((packed));

static uint64_t kernel_start_ticks = 0;
static uint64_t kernel_ticks = 0;
static gdt_reg_t gdtr;
static struct gdt_desc gdt_descs[] = { { 0 },

	                                   { .limit = 0xffff,
	                                     .base_low = 0x0000,
	                                     .base_mid = 0x00,
	                                     .access = 0b10011011,
	                                     .granularity = 0b00000000,
	                                     .base_hi = 0x00 },

	                                   { .limit = 0xffff,
	                                     .base_low = 0x0000,
	                                     .base_mid = 0x00,
	                                     .access = 0b10010011,
	                                     .granularity = 0b00000000,
	                                     .base_hi = 0x00 },

	                                   { .limit = 0xffff,
	                                     .base_low = 0x0000,
	                                     .base_mid = 0x00,
	                                     .access = 0b10011011,
	                                     .granularity = 0b11001111,
	                                     .base_hi = 0x00 },

	                                   { .limit = 0xffff,
	                                     .base_low = 0x0000,
	                                     .base_mid = 0x00,
	                                     .access = 0b10010011,
	                                     .granularity = 0b11001111,
	                                     .base_hi = 0x00 },

	                                   { .limit = 0x0000,
	                                     .base_low = 0x0000,
	                                     .base_mid = 0x00,
	                                     .access = 0b10011011,
	                                     .granularity = 0b00100000,
	                                     .base_hi = 0x00 },

	                                   { .limit = 0x0000,
	                                     .base_low = 0x0000,
	                                     .base_mid = 0x00,
	                                     .access = 0b10010011,
	                                     .granularity = 0b00000000,
	                                     .base_hi = 0x00 } };

extern void load_gdt(uint64_t gdtr);

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
	LOG("Установка сегментов\n");
	gdtr.limit = (sizeof(gdt_descs) - 1);
	gdtr.base = (uint64_t)&gdt_descs;
	load_gdt((uint64_t)&gdtr);
	LOG("Установка сегментов успешна!\n");

	pic_init( );
	idt_init( );
	cpu_init( );
}