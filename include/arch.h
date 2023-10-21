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

void arch_init( );
void cpu_init( );
void gdt_init( );
void idt_init( );

#endif // arch.h