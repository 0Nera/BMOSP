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

static inline void idt_load( ) {
	asm volatile("lidt %0" : : "m"(idtr));
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
	fb_printf("\nПОЛУЧЕНО ИСКЛЮЧЕНИЕ: %s\n", exception_names[state.int_number]);

	fb_printf("  RAX=%x  RBX=%x\n"
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
	          state.rax, state.rbx, state.rcx, state.rdx, state.rsi, state.rdi, state.rbp, state.rsp, state.r8,
	          state.r9, state.r10, state.r11, state.r12, state.r13, state.r14, state.r15, state.rip, state.rflags,
	          state.cs, state.ss, state.err, state.int_number);

	asm volatile("cli; hlt");
}

void isr_generic(struct frame state) {
	if (state.int_number > 255) { return; }

	if (state.int_number != 32) { LOG("Обработка прерывания %u\n", state.int_number); }

	if (state.int_number < 32) {
		exception_handler(state);
	} else {
		if (isr[state.int_number]) { isr[state.int_number](&state); }

		if (state.int_number >= 39) { outb(0xA0, 0x20); }
		outb(0x20, 0x20);
	}
}

void idt_init( ) {
	asm volatile("cli");
	idtr = (idt_ptr_t){ .limit = sizeof(idt) - 1, .base = (uint64_t)idt };
	tool_memset(isr, 0, 256 * sizeof(int_entry_t));

	for (uint64_t i = 0; i < 256; i++) {
		if (i < 32) {
			encode_idt_entry(i, isr_stubs[i], 0x8E);
			isr[i] = (void *)exception_handler;
		} else {
			encode_idt_entry(i, isr_stubs[i], 0x8F);
			isr[i] = (void *)isr_generic;
		}
	}

	idt_load( );
	LOG("IDT инициализирован\n");
}

void idt_set_int(uint8_t vector, int_entry_t handler) {
	isr[vector] = handler;
	idt_load( );
}