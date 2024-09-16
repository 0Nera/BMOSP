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

#include <lock.h>
#include <stdint.h>
#include <sys.h>

#define STACK_SIZE 16 * 1024 // 16 килобайт на стек

typedef struct task {
	uint64_t rax, rbx, rcx, rdx;
	uint64_t rsi, rdi, rsp, rbp;
	uint64_t cr3;

	uint64_t cpu_time;
	uint64_t cpu_time_expired;
	uint64_t id;
	char *id_str;
	void *stack;

	struct task *last;
	struct task *next;
} __attribute__((packed)) task_t;

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

extern lock_t task_lock;
extern uint64_t task_f_init;
extern task_t *current_task;

void arch_init( );
void task_init( );
void task_after_init( );
void task_switch( );
uint64_t task_new_thread(void (*func)(void *), char *name, void *arg);
void task_del_current( );
void task_del(uint64_t id);
void cpu_init( );
void pic_init( );
void pit_init( );
void idt_init( );
void idt_set_int(uint8_t vector, int_entry_t handler);
uint64_t arch_get_tick_b( );
uint64_t arch_get_tick_l( );
uint64_t arch_get_tick( );
void com_write_byte(uint8_t byte);
void com_write_bytes(char *c, uint64_t n);
time_t rtc_get_time( );

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

#define GET_TICK_BIG arch_get_tick_b( )
#define GET_TICK_lOW arch_get_tick_l( )

#endif // arch.h