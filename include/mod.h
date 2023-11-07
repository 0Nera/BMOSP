/**
 * mod.h
 * Заголовочный файл с функциями работы с памятью
 *
 * Данный заголовочный файл содержит определения, которые используются для
 * работы с памятью
 *
 */

#include <sys.h>

#ifndef MOD_H
#define MOD_H

// Максимальное количество модулей 16. Позже перепишем на динамический массив,
// сейчас для прототипа это не так важно
#define MOD_MAX 16

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

void mod_init( );
void mod_list_show( );
module_info_t *mod_find(char *tag);

#endif // mod.h