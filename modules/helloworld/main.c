#include <system.h>

const char name[] = "Привет мир!";
const char message[] = "Привет из модуля!";

module_info_t init(env_t *env) {
	return (module_info_t){ .name = (char *)&name,
		                    .message = (char *)&message,
		                    .err_code = 2023,
		                    .func_count = 1 };
}
