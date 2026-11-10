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
#include <mem.h>
#include <mod.h>
#include <sys.h>
#include <tool.h>

static volatile struct limine_module_request module_request = { .id = LIMINE_MODULE_REQUEST_ID,
	                                                            .revision = 0,
	                                                            .response = (struct limine_module_response *)0 };

static struct limine_module_response *module_response;
uint64_t modules_count = 0;
module_info_t *module_list = NULL;
static char *graphics_module_message = "Графический модуль-объект";
static char *other_module_message = "Неизвестный тип модуля";

void *bootpng_ptr;
uint64_t bootpng_size;

typedef struct {
	module_entry_t entry;
	env_t *env;
} module_start_t;

static void module_start(void *arg) {
	module_start_t *start = (module_start_t *)arg;
	start->entry(start->env);
}

// Вывод списка модулей в отладчик
void mod_list_show( ) {
	LOG("Список модулей:\n");
	for (uint64_t i = 0; i < modules_count; i++) {
		LOG("Имя: %s | ID: %u \n", module_list[i].name ? module_list[i].name : "(NULL)", i);
		LOG("Описание модуля: %s\n", module_list[i].message ? module_list[i].message : "(NULL)");
		LOG("Тип модуля: %u\n", module_list[i].type);
		LOG("Код ошибки модуля: %u\n", module_list[i].err_code);

		if (module_list[i].data_size) {
			LOG("Размер данных: %u\n", module_list[i].data_size);
			LOG("Адрес данных: 0x%x\n", module_list[i].data ? module_list[i].data : 0);
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
		if (module_list[i].name) {
			if (tool_str_contains(module_list[i].name, tag)) { return &module_list[i]; }
		}
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
		tool_memset(module_list, 0, module_count * sizeof(module_info_t));
		LOG("module_list = 0x%x\n", module_list);
	}

	for (uint64_t i = 0; i < module_count; i++) {
		module_ptr = module_response->modules[i];

		LOG("[%u] %s [%s] 0x%x Размер: %u\n", i, module_ptr->path, module_ptr->string, module_ptr->address,
		    module_ptr->size);

		if (modules_count >= module_count) {
			LOG("Модуль не обработан. Максимум %u модулей!\n", module_count);
			break;
		}

		module_list[modules_count].name = module_ptr->string;
		module_list[modules_count].message = other_module_message;

		if (tool_starts_with(module_ptr->string, "[BOOTIMG]")) {
			module_list[modules_count].data_size = module_ptr->size;
			module_list[modules_count].data = module_ptr->address;
			bootpng_ptr = module_ptr->address;
			bootpng_size = module_ptr->size;
			module_list[modules_count].type = 1; // Графика
			module_list[modules_count].message = graphics_module_message;
			modules_count++;
			continue;
		}

		if (!tool_starts_with(module_ptr->string, "[MOD]")) {
			module_list[modules_count].data_size = module_ptr->size;
			module_list[modules_count].data = module_ptr->address;
			module_list[modules_count].type = 255; // Неизвестный тип модуля
			modules_count++;
			continue;
		}

		module_entry_t module_init = (module_entry_t)elf_entry(module_ptr->address);

		if (module_init == NULL) {
			LOG("Модуль %s неисправен\n", module_ptr->string);
			continue;
		}

		// LOG("\t->Точка входа: 0x%x\n", module_init);
		uint64_t env_alloc_size = ALIGN_UP(sizeof(env_t), sizeof(uint64_t));
		uint8_t *env_mem = (uint8_t *)mem_alloc(env_alloc_size + sizeof(module_start_t));
		if (env_mem == NULL) {
			LOG("Ошибка выделения памяти для env модуля %s\n", module_ptr->string);
			continue;
		}
		env_t *env = (env_t *)env_mem;
		module_start_t *start = (module_start_t *)(env_mem + env_alloc_size);
		tool_memset(env_mem, 0, env_alloc_size + sizeof(module_start_t));
		env->offset = (uint64_t)module_ptr->address;
		env->id = modules_count;
		sys_install(env);

		start->entry = module_init;
		start->env = env;

		module_list[modules_count].env = (void *)env;
		task_set_module_context(&module_list[modules_count]);
		task_new_thread(module_start, module_ptr->string, start);
		task_set_module_context(NULL);
		modules_count++;
	}
	LOG("Модулей обработано: %u\n", modules_count);
}

void mod_update_info(env_t *env) {
	if (env == NULL || env->ret == NULL) {
		LOG("mod_update_info: пустой env или ret\n");
		return;
	}

	if (env->abi_magic != BMOSP_ABI_MAGIC || env->abi_version != BMOSP_ABI_VERSION || env->abi_size != sizeof(env_t)) {
		LOG("mod_update_info: несовместимый ABI env\n");
		return;
	}

	if (env->module_info_size != sizeof(module_info_t)) {
		LOG("mod_update_info: несовместимый ABI module_info\n");
		return;
	}

	if (env->id >= modules_count) {
		LOG("mod_update_info: неверный ID модуля %u\n", env->id);
		return;
	}

	module_info_t *info = env->ret;
	module_info_t *module = &module_list[env->id];
	module->name = info->name;
	module->message = info->message;
	module->type = info->type;
	module->data_size = info->data_size;
	module->data = info->data;
	module->err_code = info->err_code;
	module->module_id = env->id;
	module->irq = info->irq;
	module->irq_handler = info->irq_handler;
	module->get_func = info->get_func;
	module->after_init = info->after_init;
	module->env = env;
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