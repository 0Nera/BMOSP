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

void *elf_entry(void *module, uint64_t size) {
	// Приводим заголовок ELF файла к типу elf64_header_t
	elf64_header_t *elf_header = (elf64_header_t *)module;

	// Выводим данные о заголовке ELF файла
	fb::printf("ELF Header:\n");
	fb::printf("  Magic number: ");
	for (int i = 0; i < 16; i++) {
		fb::printf("0x%x ", elf_header->e_ident[i]);
	}
	fb::printf("\n");
	fb::printf("  Class:       ELF64\n");
	fb::printf("  Version:     %u\n", elf_header->e_ident[6]);
	fb::printf("  OS/ABI:      %u\n", elf_header->e_ident[7]);
	fb::printf("  Type:        %u\n", elf_header->e_type);
	fb::printf("  Machine:     %u\n", elf_header->e_machine);
	fb::printf("  Version:     %u\n", elf_header->e_version);
	fb::printf("  Entry point: 0x%x\n", elf_header->e_entry);
	fb::printf("  Program header offset:      %u\n", elf_header->e_phoff);
	fb::printf("  Section header offset:      %u\n", elf_header->e_shoff);
	fb::printf("  Flags:       %u\n", elf_header->e_flags);
	fb::printf("  ELF header size:           %u (bytes)\n",
	           elf_header->e_ehsize);
	fb::printf("  Program header entry size:  %u (bytes)\n",
	           elf_header->e_phentsize);
	fb::printf("  Program header count:      %u\n", elf_header->e_phnum);
	fb::printf("  Section header entry size:  %u (bytes)\n",
	           elf_header->e_shentsize);
	fb::printf("  Section header count:      %u\n", elf_header->e_shnum);
	fb::printf("  Section header string index: %u\n", elf_header->e_shstrndx);

	// Возвращаем указатель на точку входа
	return (void *)elf_header->e_entry;
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
	module_response = module_request.response;
	module_count = module_response->module_count;
	struct limine_file *module_ptr = (struct limine_file *)0;

	for (uint64_t i = 0; i < module_count; i++) {
		module_ptr = module_response->modules[i];
		fb::printf("[%d] %s [%s] 0x%x\n", i, module_ptr->path,
		           module_ptr->cmdline, module_ptr->address);
		fb::printf("->Size: %u, media_type: %u, partition_index: %u\n",
		           module_ptr->size, module_ptr->media_type,
		           module_ptr->partition_index);
		fb::printf("->mbr_disk_id: %u, tftp_ip: %u, tftp_port: %u\n",
		           module_ptr->mbr_disk_id, module_ptr->tftp_ip,
		           module_ptr->tftp_port);

		fb::printf("\t->Entry point: 0x%x\n\n",
		           elf_entry(module_ptr->address, module_ptr->size));
	}
}
} // namespace mod