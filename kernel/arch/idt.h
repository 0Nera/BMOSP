#include <stdint.h>

#define NO_NAME "Не задано название"

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

#define IDT_SIZE 256
#define IDT_EXCEPTIONS 32

#define IDT_DPL(x) (((unsigned)(x)&0x3u) << 13)
#define IDT_KERNEL IDT_DPL(0)

#define IDT_TYPE(x) (((unsigned)(x)&0xfu) << 8)
#define IDT_INT_GATE IDT_TYPE(0xeu)
#define IDT_TRP_FATE IDT_TYPE(0xfu)

#define IDT_PRESENT (1u << 15)

#define IDT_EXCEPTION_FLAGS (IDT_KERNEL | IDT_INT_GATE | IDT_PRESENT)
#define IDT_INTERRUPT_FLAGS (IDT_KERNEL | IDT_INT_GATE | IDT_PRESENT)
#define IDT_SPURIOUS_FLAGS (IDT_KERNEL | IDT_INT_GATE | IDT_PRESENT)

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

typedef void (*exception_handler_t)(void);
typedef void (*interrupt_handler_t)(void);

struct idt_desc {
	uint16_t offs0;
	uint16_t sel;
	uint16_t flags;
	uint16_t offs1;
	uint32_t offs2;
	uint32_t _reserved;
} __attribute__((packed));

struct idt_ptr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

struct int_desc {
	interrupt_handler_t handler;
	int busy;
};

typedef void (*int_entry_t)(void);
extern int_entry_t isr_stubs[];