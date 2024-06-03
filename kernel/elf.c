/**
 * elf.c
 * Функции работы с ELF64
 *
 * Инструменты для парсинга и анализа ELF файлов
 *
 */

#include <mod.h>
#include <stdint.h>

elf64_header_t *elf64_get_header(void *data) {
	return (elf64_header_t *)(data);
}

static inline elf64_section_header_t *elf64_sheader(elf64_header_t *hdr) {
	return (elf64_section_header_t *)((elf64_addr_t)hdr + hdr->e_shoff);
}

static inline elf64_section_header_t *elf64_section(elf64_header_t *hdr, elf64_offset_t idx) {
	return &elf64_sheader(hdr)[idx];
}

static inline char *elf64_str_table(elf64_header_t *hdr) {
	if (hdr->e_shstrndx == 0x0) return NULL;
	return (char *)hdr + elf64_section(hdr, hdr->e_shstrndx)->sh_offset;
}

static inline char *elf64_lookup_string(elf64_header_t *hdr, elf64_offset_t offset) {
	char *strtab = elf64_str_table(hdr);
	if (strtab == NULL) return NULL;
	return strtab + offset;
}

static elf64_sym_t *elf64_get_symval(elf64_header_t *hdr, elf64_offset_t table, elf64_offset_t idx) {
	if (table == 0 || idx == 0) return 0;
	elf64_section_header_t *symtab = elf64_section(hdr, table);

	uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;
	if (idx >= symtab_entries) {
		LOG("Индекс символа вне допустимых пределов (%u:%u).\n", table, idx);
		return NULL;
	}

	uintptr_t symaddr = (uint64_t)hdr + symtab->sh_offset;
	return (elf64_sym_t *)&((elf64_sym_t *)symaddr)[idx];
}

unsigned long elf64_hash(unsigned char *name) {
	unsigned long h = 0, g;
	// Вычисление хэша
	while (*name) {
		h = (h << 4) + *name++;
		// Проверка на overflow
		if (g = h & 0xf0000000) h ^= g >> 24;
		// Ограничение хэша
		h &= 0xffffffff;
	}
	return h;
}

// Получение адреса точки входа
void *elf_entry(void *module_bin) {
	// Приводим заголовок ELF файла к типу elf64_header_t
	elf64_header_t *elf_header = (elf64_header_t *)module_bin;

	LOG("(uint64_t)elf_header->e_entry = 0x%x, тип = %u\n", (uint64_t)elf_header->e_entry, elf_header->e_type);

	if (elf_header->e_type != 2) {
		LOG("\t\tОшибка! Модуль неправильно собран!\n");
		for (;;) { asm volatile("pause"); }
	}
	elf_parse((elf64_header_t *)module_bin);

	// Возвращаем указатель на точку входа
	return (void *)((uint64_t)elf_header->e_entry + (uint64_t)module_bin);
}

void *elf_parse(elf64_header_t *head) {
	//	elf64_section_header_t *symtab = NULL;

	if (head->e_ident[0] != ELFMAG0 || head->e_ident[1] != ELFMAG1 || head->e_ident[2] != ELFMAG2 ||
	    head->e_ident[3] != ELFMAG3) {
		LOG("Ошибка: Неправильный формат!\n");
		return (void *)-1;
	}

	LOG("Точка входа: 0x%x\n", head->e_entry);

	elf64_section_header_t *symtab_section = NULL;
	char *string_table = NULL;
	for (int i = 0; i < head->e_shnum; i++) {
		elf64_section_header_t *shdr = elf64_section(head, i);
		if (shdr->sh_type == SHT_SYMTAB) {
			symtab_section = shdr;
			elf64_section_header_t *strtab_section = elf64_section(head, shdr->sh_link);
			string_table = (char *)head + strtab_section->sh_offset;
			break;
		}
	}

	if (symtab_section && string_table) {
		LOG("\nТаблица символов:\n");
		LOG("%s %s %s %s\n", "Индекс", "Значение", "Размер", "Наименование");

		int num_symbols = symtab_section->sh_size / symtab_section->sh_entsize;
		for (int i = 0; i < num_symbols; i++) {
			elf64_sym_t *sym = elf64_get_symval(head, symtab_section - elf64_sheader(head), i);
			if (sym) { LOG("%6u %8x %6x %s\n", i, sym->st_value, sym->st_size, string_table + sym->st_name); }
		}
	} else {
		LOG("Таблица символов не найдена!\n");
	}

	return (void *)0;
}