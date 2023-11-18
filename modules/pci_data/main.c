#include <system.h>

typedef struct {
	char *name;
	uint16_t id;
} vendor_t;

vendor_t vendor_list[] = { { "Intel", 0x8086 },
	                       { "AMD", 0x1002 },
	                       { "AMD", 0x1002 } };

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);
	return (module_info_t){
		.name = (char *)"[PCI][DATA]",
		.message = (char *)"PCI данные",
		.type = 0,
		.data_size = 3,
		.data = &vendor_list,
		.err_code = 0,
		.module_id = 0,
	};
}