#include <system.h>

const char name[] = "Привет мир!";
const char message[] = "Привет из модуля!";
module_info_t info;
env_t *env;

void hello( ) {}
module_func_t func_table[] = { "hello", hello };

module_info_t *_start(env_t *env) {
	fb_printf = env->fb_printf;
	fb_printf("[hello]message=[%s]\n", message);

	info.name = (char *)&name;
	info.message = (char *)&message;
	info.func_count = 1;
	info.func = &func_table;
	info.err_code = 2023;

	return &info;
}