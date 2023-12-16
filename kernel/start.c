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

	pit_init( );
	mod_init( );

	fb_set_text_color(0x00FF00);
	fb_printf("Готово! Для выхода из симуляции удерживайте: ESCAPE ");
	fb_set_text_color(0xFF0000);
	fb_printf("(Требуется модуль ps2.ko)\n");
	fb_set_text_color(0x00D000);

	asm volatile("sti");

	for (;;) { asm volatile("hlt"); }
}