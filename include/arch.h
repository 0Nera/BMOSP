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

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

#endif // arch.h