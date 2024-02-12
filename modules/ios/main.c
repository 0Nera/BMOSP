#include <system.h>

static module_info_t *mod_list = NULL;
static uint64_t *mod_count = NULL;
static uint64_t app_count = 0;
static module_info_t *app_list = NULL;
static char (*getc)( ) = NULL;

static inline int is_digit(char c) {
	if (c >= '0' && c <= '9') { return 1; }
	return 0;
}

static int64_t char_to_digit(char c) {
	if (is_digit(c)) { return (int64_t)(c - '0'); }
	return -1;
}

void ios_main( ) {
	module_info_t *kbd_mod = get_module("[KEYBOARD]");

	if (kbd_mod == NULL) {
		fb_printf("Клавиатура не найдена!\n");
		delete_thread( );
		for (;;) { asm volatile("hlt"); }
	}

	getc = kbd_mod->get_func(2);

	while (1) {
		fb_printf("Доступные программы:\n");
		for (uint64_t i = 0; i < app_count; i++) { fb_printf(" %2u. %s\n", i, app_list[i].name); }
		fb_printf(" %2u. Выход\n", app_count + 1);

		fb_printf("[IOS]>");

		char c = '\0';

		do { c = getc( ); } while (!is_digit(c));

		fb_printf(" %c\n", c);

		int select = char_to_digit(c);

		if (select == app_count + 1) {
			fb_printf("Выход\n");
			delete_thread( );
			for (;;) { asm volatile("hlt"); }
		}

		if (select > app_count - 1) {
			fb_printf("Ошибка! %u не входит в список\n");
			continue;
		}

		fb_printf("Запуск %s...\n", app_list[select].name);
		int (*app)( ) = (int (*)( ))app_list[select].data;
		int ret = (*app)( );
		fb_printf("\nПриложение %s завершилось с кодом: %d\n", app_list[select].name, ret);
	}
}

static void main( ) {
	fb_printf("IOS (input-output shell) - оболочка ввода-вывода\n");
	mod_count = alloc(sizeof(uint64_t));
	mod_list = mod_list_get(mod_count);

	app_list = alloc((*mod_count) * sizeof(module_info_t));

	if (app_list == NULL) {
		fb_printf("Ошибка выделения памяти для app_list!\n");
		delete_thread( );
		for (;;) { asm volatile("hlt"); }
	}

	app_count = 0;

	for (uint64_t i = 0; i < *mod_count; i++) {
		if (str_contains(mod_list[i].name, "[APP]")) {
			// fb_printf("%u. %s\n", app_count, mod_list[i].name);
			app_list[app_count] = mod_list[i];
			app_count++;
		}
	}

	free(mod_count);

	if (app_count < 1) {
		fb_printf("Модулей-программ не обнаружено!\n");
		free(app_list);
		delete_thread( );
	} else {
		app_list = realloc(app_list, app_count * sizeof(module_info_t));
		ios_main( );
		for (;;) { asm volatile("hlt"); }
	}

	for (;;) { asm volatile("hlt"); }
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);

	return (module_info_t){ .name = (char *)"[IOS]",
		                    .message = (char *)"IOS (input-output shell) - оболочка ввода-вывода.",
		                    .type = 0,
		                    .data_size = 0,
		                    .data = (void *)0,
		                    .err_code = 0,
		                    .module_id = 0,
		                    .irq = 0,
		                    .irq_handler = 0,
		                    .get_func = 0,
		                    .after_init = main };
}
