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

uint64_t full_init = 0;

void finally( ) {
	LOG("Готово! Для выхода из симуляции удерживайте: ESCAPE\n");
	for (;;) { asm volatile("hlt"); }
}

// Точка входа
void _start( ) {
	asm volatile("cli");

	log_init( );
	mem_init( );
	fb_init( );
	log_init_mem( );
	arch_init( );
	mod_init( );
	mod_after_init( );

	LOG("\t\t\t\t *** Базовая Модульная Платформа Операционных Систем "
	    "версии %u.%u.%u %s***\n",
	    VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, KERNEL_GIT_TAG);

	LOG("\t\t\t\t *** Дата сборки: %s %s ***\n", __DATE__, __TIME__);

	pit_init( );
	task_init( );

	task_new_thread(finally);

	full_init = 1;

	asm volatile("sti");

	for (;;) { asm volatile("hlt"); }
}