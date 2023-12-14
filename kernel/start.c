/**
 * start.c
 * Файл с точкой входа
 *
 *
 * Файл с функцией для инициализации системы
 *
 */

#include <arch.h>
#include <fb.h>
#include <limine.h>
#include <mem.h>
#include <mod.h>
#include <tool.h>
#include <version.h>

// Точка входа
void _start( ) {
	asm volatile("cli");

	fb_init( );
	arch_init( );
	mem_init( );

	fb_set_text_color(0x00FF00);
	fb_printf("\t\t\t\t *** Базовая Модульная Платформа Операционных Систем "
	          "версии %u.%u.%u %s***\n",
	          VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, KERNEL_GIT_TAG);

	fb_printf("\t\t\t\t *** Дата сборки: %s %s ***\n", __DATE__, __TIME__);
	fb_set_text_color(0x00D000);

	mod_init( );
	pit_init( );

	fb_set_text_color(0x00FF00);
	LOG("Готово! Для выхода из симуляции удерживайте: ESCAPE\n");
	fb_set_text_color(0x00D000);

	asm volatile("sti");

	for (;;) {
		if (!(inb(0x64) & 1)) {
			io_wait( );
			continue;
		}
		uint64_t byte = inb(0x60);
		switch (byte) {
			case 0x1: // Клавиша "ESCAPE"
				LOG("Выход для Bochs\n");
				outw(0xB004, 0x2000);

				LOG("Выход для Qemu\n");
				outw(0x604, 0x2000);

				LOG("Выход для Virtualbox\n");
				outw(0x4004, 0x3400);

				LOG("Выход для облачного гипервизора\n");
				outw(0x600, 0x34);
				break;
			case 0x4F: // Клавиша "END"
				LOG("Вызов прерывания переключения задач!\n");
				asm volatile("int $32");
				break;
			default: break;
		}

		uint8_t status = inb(0x61);

		status |= 1;

		outb(0x61, status);
	}
}