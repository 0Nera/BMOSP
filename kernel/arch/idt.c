/**
 * idt.c
 * Инициализация обработчика прерываний
 *
 * Настройка обработчика прерываний и системных исключений
 *
 */

#include "idt.h"
#include <arch.h>
#include <fb.h>
#include <stdbool.h>
#include <stdint.h>
#include <tool.h>

static struct idt_desc IDT[IDT_SIZE] __attribute__((aligned(16)));
struct idt_ptr IDT_POINT = { .limit = sizeof(IDT) - 1, .base = (uint64_t)IDT };

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

void exception_handler(struct frame state) {
	LOG("\nПОЛУЧЕНО ИСКЛЮЧЕНИЕ: %s\n", exception_names[state.int_number]);

	uintptr_t rsp = state.rsp;

	const uintptr_t stack_bottom = rsp & ~((uintptr_t)4096 - 1);
	const uintptr_t stack_top = stack_bottom + 4096;

	LOG("  RAX=%x  RBX=%x\n"
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
	    state.r12, state.r13, state.r14, state.r15, state.rip, state.rflags,
	    state.cs, state.ss, state.err, state.int_number);

	asm volatile("cli; hlt");
}

static void idt_desc_setup(struct idt_desc *desc, unsigned sel, uintptr_t offs,
                           unsigned flags) {
	desc->offs0 = offs & 0xfffful;
	desc->offs1 = (offs >> 16) & 0xfffful;
	desc->offs2 = (offs >> 32) & 0xfffffffful;

	desc->sel = sel;
	desc->flags = flags;
	desc->_reserved = 0;
}

static void idt_load( ) {
	struct idt_ptr *ptr = &IDT_POINT;
	asm volatile("lidt %0" : : "m"(*ptr));
}

void idt_set_int(uint8_t vector, void *int_handler) {}

void idt_init( ) {
	asm volatile("sti");
	fb_printf("Настройка прерываний...\n");

	for (int i = 0; i != IDT_EXCEPTIONS; ++i) {
		const uintptr_t handler = (uintptr_t)isr_stubs[i];

		idt_desc_setup(&IDT[i], KERNEL_CS, handler, IDT_EXCEPTION_FLAGS);
	}

	for (int i = IDT_EXCEPTIONS; i != IDT_SIZE; ++i) {
		const uintptr_t handler = (uintptr_t)isr_stubs[i];

		idt_desc_setup(&IDT[i], KERNEL_CS, handler, IDT_INTERRUPT_FLAGS);
	}

	idt_desc_setup(&IDT[255], KERNEL_CS, (uintptr_t)isr_stubs[255],
	               IDT_SPURIOUS_FLAGS);

	idt_load( );
	LOG("IDT инициализирован\n");
}