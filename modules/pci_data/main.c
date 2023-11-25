#include <system.h>
typedef struct {
	char *name;
	uint16_t id;
} vendor_t;
/*
vendor_t *parse_file(char *str, int *num_vendors) {
    vendor_t *vendor_list = NULL;
    int size = 0;

    char *token;
    char *line = strtok(str, "\n");

    while (line != NULL) {
        vendor_list = realloc(vendor_list, (size + 1) * sizeof(vendor_t));
        token = strtok(line, ";");
        vendor_list[size].id = (uint16_t)strtol(token, NULL, 16);
        token = strtok(NULL, ";");
        vendor_list[size].name = strdup(token);
        size++;
        line = strtok(NULL, "\n");
    }

    *num_vendors = size;
    return vendor_list;
}*/

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);

	module_info_t *pci_data = get_module("[PCI][DATA][VENDORS]");

	if (pci_data == NULL) { fb_printf("Модуль PCI данных не найден!\n"); }
	char *str = pci_data->data;

	int num_vendors = count_chars(str, ';');
	// vendor_t *vendor_list = parse_file(str, &num_vendors);
	vendor_t *vendor_list;
	fb_printf("Количество вендоров: %u\n", num_vendors);
	return (module_info_t){
		.name = (char *)"[PCI][DATA]",
		.message = (char *)"PCI данные",
		.type = 0,
		.data_size = num_vendors,
		.data = vendor_list,
		.err_code = 0,
		.module_id = 0,
	};
}