#include <system.h>

const char name[] = "Привет мир!";
const char message[] = "Привет из модуля!";
module_info_t static_info = {
	.name = (char *)&name, .message = (char *)&message, .err_code = 2023, .func_count = 1
};

uint64_t init(env_t *env) {
	init_env(env);
	env->fb_printf("Модуль \"Привет мир!\" инициализирован!\n");
	return 0;
}
