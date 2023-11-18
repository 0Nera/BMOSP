#include <system.h>

static const char name[] = "Привет мир!";
static const char message[] = "Привет из модуля!";

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);
	fb_printf("[%s]\n", message);
	return (module_info_t){
		.name = (char *)&name,
		.message = (char *)&message,
		.type = 0,
		.data_size = 0,
		.data = (void *)0,
		.err_code = 0,
		.module_id = 0,
	};
}
