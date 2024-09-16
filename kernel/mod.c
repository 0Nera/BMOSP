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

static env_t *main_env = NULL;

void *bootpng_ptr;
uint64_t bootpng_size;

// Вывод списка модулей в отладчик
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

// Запуск модулей имеющих дополнительную точку входа
void mod_after_init( ) {
	for (uint64_t i = 0; i < modules_count; i++) {
		if (module_list[i].after_init != 0) {
			LOG("%s.after_init( );\n", module_list[i].name);
			task_new_thread(module_list[i].after_init, module_list[i].name, NULL);
		}
	}
}

// Запуск модулей имеющих дополнительную точку входа
module_info_t *mod_list_get(uint64_t *count) {
	*count = modules_count;
	return module_list;
}

// Поиск модуля по тегу
module_info_t *mod_find(char *tag) {
	for (uint64_t i = 0; i < modules_count; i++) {
		if (tool_str_contains(module_list[i].name, tag)) { return &module_list[i]; }
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
		LOG("module_list = 0x%x\n", module_list);
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

		module_info_t (*module_init)(env_t * env) = (module_info_t(*)(env_t * env)) elf_entry(module_ptr->address);

		if (module_init == NULL) {
			LOG("Модуль %s неисправен\n", module_ptr->cmdline);
			continue;
		}

		// LOG("\t->Точка входа: 0x%x\n", module_init);
		main_env = (env_t *)mem_alloc(sizeof(env_t));
		tool_memset(main_env, 0, sizeof(env_t));
		main_env->offset = (uint64_t)module_ptr->address;

		sys_install(main_env);

		uint64_t id = task_new_thread((void (*)(void *))module_init, module_list[i].name, main_env);
		module_list[modules_count].env = (void *)main_env;
		module_list[modules_count].name = 0;
		module_list[modules_count].message = 0;
		module_list[modules_count].data_size = 0;
		module_list[modules_count].data = 0;
		module_list[modules_count].get_func = 0;
		module_list[modules_count].after_init = 0;
		modules_count++;
	}
	LOG("Модулей обработано: %u\n", modules_count);
}

// Добавление модуля
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

// Удаление модуля
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