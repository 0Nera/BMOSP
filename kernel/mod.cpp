#include <fb.h>
#include <limine.h>
#include <mod.h>

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

typedef struct {
	void (*fb_printf)(char *str, ...);
} env_t;

env_t main_env;

void *elf_entry(void *module_bin, uint64_t size) {
	// Приводим заголовок ELF файла к типу elf64_header_t
	elf64_header_t *elf_header = (elf64_header_t *)module_bin;

	fb::printf("  Класс:       ELF64\n");
	fb::printf("  Версия:      %u\n", elf_header->e_ident[6]);
	fb::printf("  ОС/ABI:      %u\n", elf_header->e_ident[7]);
	fb::printf("  Тип:         %u\n", elf_header->e_type);
	fb::printf("  Машина:      %u\n", elf_header->e_machine);
	fb::printf("  Версия:      %u\n", elf_header->e_version);
	fb::printf("  Точка входа: 0x%x\n", elf_header->e_entry);
#if 0
	fb::printf("  Смещение таблицы программ:      %u\n", elf_header->e_phoff);
	fb::printf("  Смещение таблицы секций:        %u\n", elf_header->e_shoff);
	fb::printf("  Флаги:       %u\n", elf_header->e_flags);
	fb::printf("  Размер заголовка ELF файла:      %u (байт)\n",
	           elf_header->e_ehsize);
	fb::printf("  Размер записи таблицы программ:  %u (байт)\n",
	           elf_header->e_phentsize);
	fb::printf("  Количество записей таблицы программ:      %u\n",
	           elf_header->e_phnum);
	fb::printf("  Размер записи таблицы секций:  %u (байт)\n",
	           elf_header->e_shentsize);
	fb::printf("  Количество записей таблицы секций:      %u\n",
	           elf_header->e_shnum);
	fb::printf("  Индекс строки таблицы секций: %u\n", elf_header->e_shstrndx);
#endif

	// Возвращаем указатель на точку входа
	return (void *)((uint64_t)elf_header->e_entry + (uint64_t)module_bin);
}

namespace mod {
static volatile struct limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 0,
	.response = (struct limine_module_response *)0
};

struct limine_module_response *module_response;
static uint64_t module_count = 0;

void init( ) {
	main_env.fb_printf = &fb::printf;
	module_response = module_request.response;
	module_count = module_response->module_count;
	struct limine_file *module_ptr = (struct limine_file *)0;

	for (uint64_t i = 0; i < module_count; i++) {
		module_ptr = module_response->modules[i];
		fb::printf("[%d] %s [%s] 0x%x\n", i, module_ptr->path,
		           module_ptr->cmdline, module_ptr->address);
		fb::printf("->Размер: %u, тип носителя: %u, индекс раздела: %u\n",
		           module_ptr->size, module_ptr->media_type,
		           module_ptr->partition_index);
		fb::printf(
		    "->Идентификатор диска MBR: %u, TFTP IP: %u, TFTP порт: %u\n",
		    module_ptr->mbr_disk_id, module_ptr->tftp_ip,
		    module_ptr->tftp_port);

		long long (*module_init)(env_t * env) = (long long (*)(env_t * env))
		    elf_entry(module_ptr->address, module_ptr->size);

		fb::printf("\t->Точка входа: 0x%x\n", module_init);

		int ret = module_init(&main_env);

		fb::printf("Инициализированно с кодом: %x\n", ret);
		fb::printf("Сообщение из модуля: %s\n\n", (char *)ret);
	}
}
} // namespace mod