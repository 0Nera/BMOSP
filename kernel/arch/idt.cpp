#include <arch.h>
#include <fb.h>
#include <stdbool.h>
#include <stdint.h>
#include <tool.h>

extern "C" {
typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint64_t base;
} idt_ptr_t;

typedef struct __attribute__((packed)) {
	uint16_t offset_16;
	uint16_t selector;
	uint8_t ist;
	uint8_t flags;
	uint16_t offset_middle_16;
	uint32_t offset_high_32;
	uint32_t reserved;
} idt_gate_t;

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

static idt_gate_t idt[256];
void *isr[256];
extern void *isr_stubs[];
static idt_ptr_t idtr;

#define NO_NAME "Не задано название"

const char *exception_names[] = { "Деление на ноль",
	                              "Отладка",
	                              "NMI",
	                              "Точка останова",
	                              "Переполнение",
	                              "Выход за границы",
	                              "Недопустимая операция",
	                              "Устройство недоступно",
	                              "Двойное исключение",
	                              NO_NAME,
	                              "Недопустимый TSS",
	                              "Сегмент не присутствует",
	                              "Ошибка сегмента стека",
	                              "Общая защитная ошибка",
	                              "Ошибка страницы",
	                              NO_NAME,
	                              "x87 исключение",
	                              "Проверка выравнивания",
	                              "Ошибка машины",
	                              "SIMD исключение",
	                              "Виртуализация",
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              "Безопасность" };

void idt_load( ) {
	asm volatile("lidt %0" : : "m"(idtr));
	asm volatile("sti");
}

static void encode_idt_entry(uint8_t vector, void *handler, uint8_t flags) {
	uint64_t ptr = (uint64_t)handler;

	idt[vector].offset_16 = (uint16_t)ptr;
	idt[vector].selector = 0x28;
	idt[vector].ist = 0;
	idt[vector].flags = flags;
	idt[vector].offset_middle_16 = (uint16_t)(ptr >> 16);
	idt[vector].offset_high_32 = (uint32_t)(ptr >> 32);
	idt[vector].reserved = 0;
}

static void exception_handler(struct frame state) {
	fb::printf("\nПОЛУЧЕНО ИСКЛЮЧЕНИЕ: %s\n",
	           exception_names[state.int_number]);

	fb::printf("  RAX=%x  RBX=%x\n"
	           "  RCX=%x  RDX=%x\n"
	           "  RSI=%x  RDI=%x\n"
	           "  RBP=%x  RSP=%x\n"
	           "  R08=%x  R09=%x\n"
	           "  R10=%x  R11=%x\n"
	           "  R12=%x  R13=%x\n"
	           "  R14=%x  R15=%x\n"
	           "  RIP=%x  RFLAGS=%x\n"
	           "  CS=%x SS=%x\n"
	           "  ERR=%x  INT=%u",
	           state.rax, state.rbx, state.rcx, state.rdx, state.rsi, state.rdi,
	           state.rbp, state.rsp, state.r8, state.r9, state.r10, state.r11,
	           state.r12, state.r13, state.r14, state.r15, state.rip,
	           state.rflags, state.cs, state.ss, state.err, state.int_number);

	asm volatile("cli; hlt");
}

void isr_generic(struct frame state) {
	if (state.int_number < 32) {
		exception_handler(state);
	} else {
		fb::printf("\nПрерывание! %u необработано :(\n", state.int_number);
	}
}

void idt_init( ) {
	idtr = (idt_ptr_t){ .limit = sizeof(idt) - 1, .base = (uint64_t)idt };

	for (uint64_t i = 0; i < 256; i++) {
		if (i < 32) {
			encode_idt_entry(i, isr_stubs[i], 0x8e);
			isr[i] = (void *)exception_handler;
		} else {
			encode_idt_entry(i, isr_stubs[i], 0x8e);
			isr[i] = (void *)isr_generic;
		}
	}

	idt_load( );
	fb::printf("IDT инициализирован.\n");
}

void idt_set_ist(uint8_t vector, uint8_t ist) {
	idt[vector].ist = ist;
}
}
