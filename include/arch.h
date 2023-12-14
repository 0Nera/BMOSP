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

struct frame {
	uint64_t rbp;
	uint64_t rbx;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rax;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t int_number;
	uint64_t err;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} __attribute__((packed));

typedef void (*int_entry_t)(struct frame *state);

void arch_init( );
void cpu_init( );
void gdt_init( );
void pic_init( );
void pit_init( );
void idt_init( );
void idt_set_int(uint8_t vector, int_entry_t handler);
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

static inline void io_wait( ) {
	outb(0x80, 0);
}

#endif // arch.h