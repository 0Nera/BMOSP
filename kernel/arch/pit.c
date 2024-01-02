/**
 * pit.c
 * Программируемый интервальный таймер
 *
 * Настройка программируемого интервального таймера и
 *
 */

#include <arch.h>
#include <fb.h>
#include <stdbool.h>
#include <stdint.h>
#include <tool.h>

extern void task_switch(struct frame *state);

void pit_set_interval(int hz) {
	int divisor = 1193180 / hz; // Вычисляем делитель
	outb(0x43, 0x34);           // Устанавливаем байт команды 0x34
	outb(0x40, divisor & 0xFF); // Устанавливаем младший байт делителя
	outb(0x40, divisor >> 8);   // Устанавливаем старший байт делителя
}

void pit_init( ) {
	idt_set_int(32, task_switch);
	pit_set_interval(1);
}