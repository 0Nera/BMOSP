#include <system.h>

typedef struct {
	char *name;
	uint16_t id;
} vendor_t;

static vendor_t **parse_file(char *str, uint64_t num_vendors, uint64_t size) {
	vendor_t **vendor_list = (vendor_t **)alloc(num_vendors * sizeof(vendor_t *));

	if (vendor_list == NULL) { return NULL; }
	for (uint64_t i = 0; i < num_vendors; i++) { vendor_list[i] = (vendor_t *)alloc(sizeof(vendor_t)); }

	uint64_t i = 0;

	char *line = alloc(strlen(str) + 1);
	char *temp = alloc(strlen(str) + 1);

	if (line == NULL) { return NULL; }
	if (temp == NULL) { return NULL; }
	strcpy(line, str);

	while (line != NULL && i < num_vendors) {
		temp = realloc(temp, strlen(line) + 1);
		strcpy(temp, line);

		char *name = trstr(temp, ';');
		char *id_str = strtok(temp, ";");

		name = strtok(name, "\n");
		name[strlen(name) - 1] = '\0';

		if (id_str != NULL && name != NULL) {
			vendor_list[i]->id = strtol(id_str, NULL, 16);
			vendor_list[i]->name = name;
			i++;
		}
		temp = trstr(line, '\n');
		strcpy(line, temp);
	}

	if (i != num_vendors) {
		for (uint64_t j = 0; j < i; j++) { free(vendor_list[j]->name); }
		free(vendor_list);
		free(line);
		free(temp);
		return NULL;
	}

	free(line);
	free(temp);
	return vendor_list;
}

static void print_vendors(uint64_t num_vendors, vendor_t **vendor_list) {
	for (uint64_t i = 0; i < num_vendors; i++) {
		vendor_t *vendor = vendor_list[i];
		fb_printf("ID: 0x%x, Name: %s\n", vendor->id, vendor->name);
	}
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);

	module_info_t *pci_data = get_module("[PCI][DATA][VENDORS]");

	if (pci_data == NULL) { fb_printf("База PCI не найдена!\n"); }

	uint64_t num_vendors = count_chars(pci_data->data, ';');
	fb_printf("Количество вендоров: %u\n", num_vendors);

	vendor_t **vendor_list = parse_file(pci_data->data, num_vendors, pci_data->data_size);
	// print_vendors(num_vendors, vendor_list);
	return (module_info_t){ .name = (char *)"[PCI][ADAPTER]",
		                    .message = (char *)"PCI данные",
		                    .type = 0,
		                    .data_size = num_vendors,
		                    .data = vendor_list,
		                    .err_code = 0,
		                    .module_id = 0,
		                    .irq = 0,
		                    .irq_handler = 0,
		                    .get_func = 0,
		                    .after_init = 0 };
}