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

	full_init = 1;

	task_after_init( );
	mem_get_stat( );

	LOG("Готово! Для выхода из симуляции удерживайте: ESCAPE\n");
	asm volatile("sti");
	for (;;) {
		task_t *task = current_task;
		do {
			task = task->next;
			if (task->status == 0) {
				// LOG("УДАЛЕНИЕ %u(%s)\n", task->id, task->id_str);
				task_t *prev = task->last;
				task_t *next = task->next;

				prev->next = next;
				next->last = prev;

				mem_free(task->stack);
				mem_free(task);
			}
		} while (task->id != 0);

		task_switch( );
	}
}
