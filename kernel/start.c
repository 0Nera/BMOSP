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
uint64_t dum = 0;

void finally( ) {
	LOG("Готово! Для выхода из симуляции удерживайте: ESCAPE\n");
	for (;;) { asm volatile("hlt"); }
}

void dummy( ) {
	LOG("Поток %u\n", dum++);
	task_del_current( );
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
	pit_init( );
	task_init( );
	task_f_init = 1;
	mod_init( );

	LOG("\t\t\t\t *** Базовая Модульная Платформа Операционных Систем "
	    "версии %u.%u.%u %s***\n",
	    VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, KERNEL_GIT_TAG);

	LOG("\t\t\t\t *** Дата сборки: %s %s ***\n", __DATE__, __TIME__);

	time_t time = rtc_get_time( );
	LOG("Время: %u:%u.%u, %u.%u.%u\n", time.hours, time.minutes, time.second, time.day, time.month, time.year);

	task_new_thread(finally, "fin");

	full_init = 1;

	task_after_init( );

	asm volatile("sti");

	for (;;) { asm volatile("hlt"); }
}