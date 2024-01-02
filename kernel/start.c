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

char (*getc)( );
extern task_t* current_task;

// Пример функции, которую будем выполнять в разных потоках
static void test_task(void* arg) {
	fb_printf("\n\t\t[%u]\n", current_task->id);
	for (;;) {}
}

char* msg = "123!!!!";
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

	task_init( );
	pit_init( );
	// mod_init( );

	fb_set_text_color(0x00FF00);
	fb_printf("Готово! Для выхода из симуляции удерживайте: ESCAPE\n");

	module_info_t* mod = mod_find("[KEYBOARD]");

	if (mod == NULL) {
		fb_set_text_color(0xFF0000);
		fb_printf("\tТребуется модуль ps2.ko!\n");
		getc = NULL;
	} else {
		fb_printf("\nОбнаружен модуль клавиатуры: %s\n", mod->name);
		getc = mod->get_func(1);
	}

	fb_set_text_color(0x00D000);

	// Создаем новый поток и передаем ему аргумент
	fb_printf("\tСоздаем новый поток и передаем ему аргумент\n");
	task_t* thread1 = task_new_thread(&test_task, &msg);
	// Создаем новый поток и передаем ему аргумент
	fb_printf("\tСоздаем новый поток и передаем ему аргумент\n");
	task_t* thread2 = task_new_thread(test_task, &msg);

	// Переключаем контекст на первый поток
	// task_switch(thread1->state);

	asm volatile("sti");

	for (;;) {
		asm volatile("hlt");

		if (getc) { fb_printf("%c", getc( )); }
	}
}