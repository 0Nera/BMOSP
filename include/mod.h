/**
 * mod.h
 * Заголовочный файл с функциями работы с памятью
 *
 * Данный заголовочный файл содержит определения, которые используются для
 * работы с памятью
 *
 */

#include <log.h>
#include <stddef.h>
#include <stdint.h>
#include <sys.h>

#ifndef MOD_H
#define MOD_H

#define EI_MAG0 0
#define ELFMAG0 0x7f
#define EI_MAG1 1
#define ELFMAG1 'E'
#define EI_MAG2 2
#define ELFMAG2 'L'
#define EI_MAG3 3
#define ELFMAG3 'F'
#define SHT_SYMTAB 2

typedef uint64_t elf64_addr_t;   // Адрес
typedef uint64_t elf64_offset_t; // Смещение
typedef uint64_t elf64_xword_t;  // Целочисленное длинное слово без знака
typedef uint64_t elf64_sxword_t; // Целочисленное длинное слово с знаком
typedef uint32_t elf64_word_t;   // Целочисленное слово без знака
typedef uint32_t elf64_sword_t;  // Целочисленное слово с знаком
typedef uint16_t elf64_half_t;   // Среднее целое число без знака
typedef uint8_t elf64_small_t;   // Малое целое число без знака

typedef struct {
	elf64_small_t e_ident[16]; // Идентификация ELF
	elf64_half_t e_type;       // Тип объектного файла
	elf64_half_t e_machine;    // Тип компьютера
	elf64_word_t e_version;    // Версия объектного файла
	elf64_addr_t e_entry;      // Адрес точки входа
	elf64_offset_t e_phoff;    // Смещение заголовка программы
	elf64_offset_t e_shoff;    // Смещение заголовка раздела
	elf64_word_t e_flags;      // Флаги, зависящие от процессора
	elf64_half_t e_ehsize;     // Размер заголовка ELF
	elf64_half_t e_phentsize;  // Размер записи заголовка программы
	elf64_half_t e_phnum;      // Количество записей в заголовке программы
	elf64_half_t e_shentsize;  // Размер записи в заголовке раздела
	elf64_half_t e_shnum;      // Количество записей в заголовке раздела
	elf64_half_t e_shstrndx;   // Строковый табличный индекс названия раздела
} elf64_header_t;

typedef struct {
	elf64_word_t sh_name;       // Название раздела
	elf64_word_t sh_type;       // Тип раздела
	elf64_xword_t sh_flags;     // Атрибуты раздела
	elf64_addr_t sh_addr;       // Виртуальный адрес в памяти
	elf64_offset_t sh_offset;   // Смещение в файле
	elf64_xword_t sh_size;      // Размер раздела
	elf64_word_t sh_link;       // Ссылка на другой раздел
	elf64_word_t sh_info;       // Дополнительная информация
	elf64_xword_t sh_addralign; // Граница выравнивания адреса
	elf64_xword_t sh_entsize;   // Размер записей, если в разделе есть таблица
} elf64_section_header_t;

typedef struct {
	elf64_addr_t r_offset; // Адрес ссылки
	elf64_xword_t r_info;  // Индекс символа и тип перемещения
} elf64_rel_t;

typedef struct {
	elf64_addr_t r_offset;   // Адрес ссылки
	elf64_xword_t r_info;    // Индекс символа и тип перемещения
	elf64_sxword_t r_addend; // Постоянная часть выражения
} elf64_rela_t;

typedef struct {
	elf64_word_t p_type;     // Тип сегмента
	elf64_word_t p_flags;    // Атрибуты сегмента
	elf64_offset_t p_offset; // Смещение в файле
	elf64_addr_t p_vaddr;    // Виртуальный адрес в памяти
	elf64_addr_t p_paddr;    // Зарезервирован
	elf64_xword_t p_filesz;  // Размер сегмента в файле
	elf64_xword_t p_memsz;   // Размер сегмента в памяти
	elf64_xword_t p_align;   // Выравнивание сегмента
} elf64_phdr_t;

typedef struct {
	elf64_word_t st_name;   // Название символа
	elf64_small_t st_info;  // Тип и атрибуты привязки
	elf64_small_t st_other; // Зарезервировано
	elf64_half_t st_shndx;  // Индекс таблицы разделов
	elf64_addr_t st_value;  // Значение символа
	elf64_xword_t st_size;  // Размер объекта (например, общий)
} elf64_sym_t;

typedef struct {
	elf64_sxword_t d_tag; // Тип динамического элемента
	union {
		elf64_xword_t d_val; // Значение динамического элемента
		elf64_addr_t d_ptr;  // Указатель динамического элемента
	} d_un;
} elf64_dyn_t;

void mod_init( );
void mod_after_init( );
void mod_list_show( );
module_info_t *mod_find(char *tag);
module_info_t *mod_list_get(uint64_t *count);

void *elf_entry(void *module_bin);
void *elf_parse(elf64_header_t *head);

#endif // mod.h