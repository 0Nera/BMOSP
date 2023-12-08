/**
 * arch.h
 * Заголовок для инициализации архитектурно-зависимых функций
 *
 * Данный заголовочный файл содержит определения которые используются для
 * инициализации архитектуры
 *
 */

#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

void arch_init( );
void cpu_init( );
void gdt_init( );
void idt_init( );
void idt_set_int(uint8_t vector, void *int_handler);
uint64_t arch_get_tick_b( );
uint64_t arch_get_tick_l( );
uint64_t arch_get_tick( );

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
	asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
	uint16_t ret;
	asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

#endif // arch.h