#include <system.h>

static module_info_t *mod_list = NULL;
static uint64_t *mod_count = NULL;
static uint64_t app_count = 0;
static module_info_t *app_list = NULL;

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
			//fb_printf("%u. %s\n", app_count, mod_list[i].name);
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
		for (uint64_t i = 0; i < app_count; i++) { fb_printf("%2u.\t%s\n", i, app_list[i].name); }
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
