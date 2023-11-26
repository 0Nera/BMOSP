#include <system.h>
typedef struct {
	char *name;
	uint16_t id;
} vendor_t;

static vendor_t *parse_file(char *str, uint64_t num_vendors, uint64_t size) {
	vendor_t *vendor_list = alloc(num_vendors * sizeof(vendor_t));
	if (vendor_list == NULL) { return NULL; }

	char *line = str;
	uint64_t i = 0;
	while (line != NULL && i < num_vendors) {
		char *name = trstr(line, ';');
		char *id_str = strtok(line, ";");
		name = strtok(name, "\n");
		fb_printf("\t%s, %s\n", id_str, name);

		if (id_str != NULL && name != NULL) {
			fb_printf("\tID: 0x%x, Name: %s\n", strtol(id_str, NULL, 16), name);
			vendor_list[i].id = strtol(id_str, NULL, 16);
			vendor_list[i].name = name;
			i++;
		}
		line = trstr(line, '\n');
		fb_printf(line);
	}

	if (i != num_vendors) {
		// Ошибка в парсинге данных
		for (uint64_t j = 0; j < i; j++) { free(vendor_list[j].name); }
		free(vendor_list);
		return NULL;
	}

	return vendor_list;
}

static void print_vendors(uint64_t num_vendors, vendor_t *vendor_list) {
	for (uint64_t i = 0; i < num_vendors; i++) {
		vendor_t *vendor = &vendor_list[i];
		fb_printf("ID: 0x%x, Name: %s\n", vendor->id, vendor->name);
	}
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);

	module_info_t *pci_data = get_module("[PCI][DATA][VENDORS]");

	if (pci_data == NULL) { fb_printf("Модуль PCI данных не найден!\n"); }
	char *str = pci_data->data;

	uint64_t num_vendors = count_chars(str, ';');

	uint64_t i = 1;
	char *line = str;
	while (line != NULL && i < num_vendors) {
		i++;
		line = trstr(line, '\n');
		fb_printf("\t\t%u\n%s\n", i, line);
	}
	fb_printf("Количество вендоров: %u\n", num_vendors);
	for (;;) {}

	vendor_t *vendor_list = parse_file(str, num_vendors, pci_data->data_size);
	// print_vendors(num_vendors, vendor_list);
	return (module_info_t){
		.name = (char *)"[PCI][ADAPTER]",
		.message = (char *)"PCI данные",
		.type = 0,
		.data_size = num_vendors,
		.data = vendor_list,
		.err_code = 0,
		.module_id = 0,
	};
}