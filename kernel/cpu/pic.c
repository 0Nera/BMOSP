/**
 * pic.c
 * Программируемый контроллер прерываний
 *
 * Настройка PIC (Программируемый контроллер прерываний)
 *
 */

#include <arch.h>
#include <log.h>

void pic_init( ) {
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	io_wait( );
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	io_wait( );
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	io_wait( );
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	io_wait( );
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
	LOG("Программируемый контроллер прерываний инициализирован\n");
}