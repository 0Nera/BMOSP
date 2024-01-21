/**
 * mod.c
 * Функции загрузки модулей
 *
 * Файл с функциями для загрузки модулей в формате ELF файлов полученных от
 * загрузчика
 *
 */

#include <fb.h>
#include <limine.h>
#include <log.h>
#include <mod.h>
#include <sys.h>
#include <tool.h>

static volatile struct limine_module_request module_request = { .id = LIMINE_MODULE_REQUEST,
	                                                            .revision = 0,
	                                                            .response = (struct limine_module_response *)0 };

static struct limine_module_response *module_response;
uint64_t modules_count = 0;
module_info_t *module_list = NULL;
static char *graphics_module_message = "Графический модуль-объект";
static char *other_module_message = "Неизвестный тип модуля";

static env_t main_env;

void *bootpng_ptr;
uint64_t bootpng_size;

static void *elf_entry(elf64_header_t *module_bin) {
	// Приводим заголовок ELF файла к типу elf64_header_t
	elf64_header_t *elf_header = (elf64_header_t *)module_bin;

	// LOG("(uint64_t)elf_header->e_entry = 0x%x, type = %u\n", (uint64_t)elf_header->e_entry, elf_header->e_type);

	if (elf_header->e_type != 2) {
		LOG("\t\tОшибка! Модуль неправильно собран!\n");
		for (;;) { asm volatile("pause"); }
	}

	// Возвращаем указатель на точку входа
	return (void *)((uint64_t)elf_header->e_entry + (uint64_t)module_bin);
}

void mod_list_show( ) {
	for (uint64_t i = 0; i < modules_count; i++) {
		LOG("Имя: %s\n", module_list[i].name);
		LOG("Описание модуля: %s\n", module_list[i].message);
		LOG("Тип модуля: %u\n", module_list[i].type);
		LOG("Код ошибки модуля: %u\n", module_list[i].err_code);

		if (module_list[i].data_size) {
			LOG("Размер данных: %u\n", module_list[i].data_size);
			LOG("Адрес данных: 0x%x\n", module_list[i].data);
		}
	}
}

void mod_after_init( ) {
	for (uint64_t i = 0; i < modules_count; i++) {
		if (module_list[i].after_init != 0) { module_list[i].after_init( ); }
	}
}

module_info_t *mod_list_get(uint64_t *count) {
	*count = modules_count;
	return module_list;
}

module_info_t *mod_find(char *tag) {
	for (uint64_t i = 0; i < modules_count; i++) {
		if (tool_starts_with(module_list[i].name, tag)) { return &module_list[i]; }
	}

	return (module_info_t *)NULL;
}

void mod_init( ) {
	module_response = module_request.response;

	uint64_t module_count = module_response->module_count;
	struct limine_file *module_ptr = (struct limine_file *)0;

	if (module_count > 0) {
		module_list = (module_info_t *)mem_alloc(module_count * sizeof(module_info_t));
		if (module_list == NULL) {
			LOG("Ошибка выделения памяти для массива module_list\n");
			return;
		}
	}

	for (uint64_t i = 0; i < module_count; i++) {
		module_ptr = module_response->modules[i];

		LOG("[%u] %s [%s] 0x%x Размер: %u\n", i, module_ptr->path, module_ptr->cmdline, module_ptr->address,
		    module_ptr->size);

		if (modules_count >= module_count) {
			LOG("Модуль не обработан. Максимум %u модулей!\n", module_count);
			break;
		}

		module_list[modules_count].name = module_ptr->cmdline;
		module_list[modules_count].message = other_module_message;

		if (tool_starts_with(module_ptr->cmdline, "[BOOTIMG]")) {
			module_list[modules_count].data_size = module_ptr->size;
			module_list[modules_count].data = module_ptr->address;
			bootpng_ptr = module_ptr->address;
			bootpng_size = module_ptr->size;
			module_list[modules_count].type = 1; // Графика
			module_list[modules_count].message = graphics_module_message;
			modules_count++;
			continue;
		}

		if (!tool_starts_with(module_ptr->cmdline, "[MOD]")) {
			module_list[modules_count].data_size = module_ptr->size;
			module_list[modules_count].data = module_ptr->address;
			module_list[modules_count].type = 255; // Неизвестный тип модуля
			modules_count++;
			continue;
		}

		module_info_t (*module_init)(env_t * env) =
		    (module_info_t(*)(env_t * env)) elf_entry((elf64_header_t *)module_ptr->address);

		// LOG("\t->Точка входа: 0x%x\n", module_init);

		main_env.offset = (uint64_t)module_ptr->address;

		sys_install(&main_env);

		module_info_t ret = module_init(&main_env);

		LOG("\t->%s\n", ret.message);
		module_list[modules_count].name = ret.name;
		module_list[modules_count].message = ret.message;
		module_list[modules_count].data_size = ret.data_size;
		module_list[modules_count].get_func = ret.get_func;
		module_list[modules_count].after_init = ret.after_init;

		if (ret.data_size != 0) { module_list[modules_count].data = ret.data; }

		if (ret.irq != 0) {
			if (ret.irq_handler != 0) {
				LOG("Установлен обработчик прерывания [%u] по адресу 0x%x в модуле %s\n", ret.irq, ret.irq_handler,
				    ret.name);
				idt_set_int(ret.irq, ret.irq_handler);
			}
		}

		modules_count++;
	}
	LOG("Модулей обработано: %u\n", modules_count);
}

void mod_add(module_info_t module) {
	if (modules_count == 0) {
		module_list = (module_info_t *)mem_alloc(sizeof(module_info_t));

		if (module_list == NULL) {
			LOG("Ошибка выделения памяти для массива module_list\n");
			return;
		}
	} else {
		module_info_t *new_module_list =
		    (module_info_t *)mem_realloc(module_list, (modules_count + 1) * sizeof(module_info_t));

		if (new_module_list == NULL) {
			LOG("Ошибка выделения памяти для массива module_list\n");
			return;
		}

		module_list = new_module_list;
	}

	module_list[modules_count] = module;
	modules_count++;
}

void mod_del(module_info_t *module) {
	if (modules_count == 0) {
		LOG("Модуль не найден\n");
		return;
	}

	for (uint64_t i = 0; i < modules_count; i++) {
		if (&module_list[i] == module) {
			for (uint64_t j = i; j < modules_count - 1; j++) { module_list[j] = module_list[j + 1]; }

			modules_count--;
			module_list = (module_info_t *)mem_realloc(module_list, modules_count * sizeof(module_info_t));

			if (module_list == NULL) {
				LOG("Ошибка выделения памяти для массива module_list\n");
				return;
			}

			LOG("Модуль удален\n");
			return;
		}
	}

	LOG("Модуль не найден\n");
}