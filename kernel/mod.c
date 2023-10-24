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
#include <mod.h>
#include <sys.h>
#include <tool.h>

// Структуры соответствующие ELF заголовкам
typedef struct {
	unsigned char e_ident[16];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} elf64_header_t;

static env_t main_env;

void *bootpng_ptr;
uint64_t bootpng_size;
void main( );

static void *elf_entry(void *module_bin, uint64_t size) {
	// Приводим заголовок ELF файла к типу elf64_header_t
	elf64_header_t *elf_header = (elf64_header_t *)module_bin;

#if 0
	fb_printf("  Класс:       ELF64\n");
	fb_printf("  Версия:      %u\n", elf_header->e_ident[6]);
	fb_printf("  ОС/ABI:      %u\n", elf_header->e_ident[7]);
	fb_printf("  Тип:         %u\n", elf_header->e_type);
	fb_printf("  Машина:      %u\n", elf_header->e_machine);
	fb_printf("  Версия:      %u\n", elf_header->e_version);
	fb_printf("  Точка входа: 0x%x\n", elf_header->e_entry);
#endif

	// Возвращаем указатель на точку входа
	return (void *)((uint64_t)elf_header->e_entry + (uint64_t)module_bin);
}

static volatile struct limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST, .revision = 0, .response = (struct limine_module_response *)0
};

static struct limine_module_response *module_response;
static uint64_t modules_count = 0;

void mod_init( ) {
	main_env.fb_printf = &fb_printf;
	module_response = module_request.response;
	uint64_t module_count = module_response->module_count;
	struct limine_file *module_ptr = (struct limine_file *)0;

	for (uint64_t i = 0; i < module_count; i++) {
		module_ptr = module_response->modules[i];
		fb_printf("[%d] %s [%s] 0x%x\n", i, module_ptr->path, module_ptr->cmdline,
		          module_ptr->address);
		fb_printf("->Размер: %u, тип носителя: %u, индекс раздела: %u\n", module_ptr->size,
		          module_ptr->media_type, module_ptr->partition_index);
#if 0
		fb_printf("->Идентификатор диска MBR: %u, TFTP IP: %u, TFTP порт: %u\n",
		          module_ptr->mbr_disk_id, module_ptr->tftp_ip, module_ptr->tftp_port);

#endif
		if (tool_starts_with(module_ptr->cmdline, "[BOOTIMG]")) {
			fb_printf("\t\t[BOOTIMG]\n");
			bootpng_ptr = module_ptr->address;
			bootpng_size = module_ptr->size;
			continue;
		}
		if (!tool_starts_with(module_ptr->cmdline, "[MOD]")) { continue; }
		modules_count++;
		uint64_t (*module_init)(env_t * env) =
		    (module_info_t * (*)(env_t * env)) elf_entry(module_ptr->address, module_ptr->size);

		fb_printf("\t->Точка входа: 0x%x\n", module_init);

		uint64_t ret = module_init(&main_env);

		fb_printf("Инициализированно с кодом: %u\n", ret);
		// fb_printf("Инициализированно с кодом: %u\n", ret->err_code);
		// fb_printf("Сообщение из модуля: %s\n\n", ret->message);
	}
	fb_printf("Модулей обработано: %u\n", modules_count);
}