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
	LOG("\t\t\t\t *** Базовая Модульная Платформа Операционных Систем "
	    "версии %u.%u.%u ***\n",
	    VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);

	LOG("\t\t\t\t *** Дата сборки: %s %s ***\n", __DATE__, __TIME__);
	fb_set_text_color(0x00D000);

	mod_init( );
	// pit_init( );

	mod_list_show( );

	fb_set_text_color(0x00FF00);
	LOG("Готово! Для выхода из симуляции удерживайте: ESCAPE\n");
	fb_set_text_color(0x00D000);

	while (1) {
		uint64_t byte = inb(0x60);
		if (byte == 0x1) {
			LOG("Exit!\n");
			return;
		}
	}
}