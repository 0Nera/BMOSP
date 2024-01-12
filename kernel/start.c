/**
 * start.c
 * Файл с точкой входа
 *
 *
 * Файл с функцией для инициализации системы
 *
 */

#include <arch.h>
#include <limine.h>
#include <log.h>
#include <mem.h>
#include <mod.h>
#include <tool.h>
#include <version.h>

// Точка входа
void _start( ) {
	asm volatile("cli");

	log_init( );
	fb_init( );
	arch_init( );
	mem_init( );
	log_init_mem( );

	LOG("\t\t\t\t *** Базовая Модульная Платформа Операционных Систем "
	    "версии %u.%u.%u %s***\n",
	    VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, KERNEL_GIT_TAG);

	LOG("\t\t\t\t *** Дата сборки: %s %s ***\n", __DATE__, __TIME__);

	task_init( );
	pit_init( );
	mod_init( );

	LOG("Готово! Для выхода из симуляции удерживайте: ESCAPE\n");

	asm volatile("sti");

	for (;;) { asm volatile("hlt"); }
}