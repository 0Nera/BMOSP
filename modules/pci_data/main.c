#include <system.h>

typedef struct {
	char *name;
	uint16_t id;
} vendor_t;

vendor_t vendor_list[] = { { "Intel", 0x8086 },
	                       { "AMD", 0x1002 },
	                       { "AMD", 0x1002 } };

module_info_t init(env_t *env) {
	init_env(env);
	return (module_info_t){ .name = (char *)"[PCI][DATA]",
		                    .message = (char *)"PCI данные",
		                    .data_size = 3,
		                    .data = &vendor_list };
}