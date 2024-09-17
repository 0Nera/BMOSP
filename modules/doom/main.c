#include "PureDOOM.h"
#include <system.h>

static const char name[] = "[APP]DOOM";
static const char message[] = "DOOM :)";

static int app_main( ) {
	log_printf("[%s]\n", message);
	return 2 + 2;
}

void __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);
	env->ret = &((module_info_t){ .name = (char *)&name,
	                              .message = (char *)&message,
	                              .type = 0,
	                              .data_size = 0,
	                              .data = (void *)&app_main,
	                              .err_code = 0,
	                              .module_id = 0,
	                              .irq = 0,
	                              .irq_handler = 0,
	                              .get_func = 0,
	                              .after_init = 0 });
	mod_update_info(env);
	delete_thread( );
}
