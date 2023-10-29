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

	LOG("\t\t\t\t *** Базовая Модульная Платформа Операционных Систем "
	    "версии %u.%u.%u ***\n",
	    VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);

	LOG("\t\t\t\t *** Дата сборки: %s %s ***\n", __DATE__, __TIME__);
	mod_init( );
	pit_init( );
	LOG("Готово!\n");
	for (;;) { asm volatile("hlt"); }
}