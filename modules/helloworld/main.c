#include <system.h>

static const char name[] = "Привет мир!";
static const char message[] = "Привет из модуля!";

module_info_t init(env_t *env) {
	init_env(env);
	fb_printf("[%s]\n", message);
	return (module_info_t){ .name = (char *)&name,
		                    .message = (char *)&message,
		                    .err_code = 2023,
		                    .func_count = 1 };
}
