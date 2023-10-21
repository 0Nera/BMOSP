/**
 * arch.c
 * Инициализация архитектуры
 *
 * Настройка архитектурнозависимых функций
 *
 */

#include <arch.h>

void arch_init( ) {
	gdt_init( );
	idt_init( );
	cpu_init( );
}