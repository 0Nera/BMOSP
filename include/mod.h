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

#define STT_NOTYPE 0  // Тип символа не указан
#define STT_OBJECT 1  // Символ является объектом данных
#define STT_FUNC 2    // Символ является объектом кода
#define STT_SECTION 3 // Символ связан с разделом
#define STT_FILE 4    // Имя символа является именем файла
#define STT_COMMON 5  // Символ является общим объектом данных
#define STT_TLS 6     // Символ является объектом данных локального потока
#define STT_NUM 7     // Количество определенных типов.
#define STT_GNU_IFUNC 10 // Символ является объектом непрямого кода

#define ELF32_ST_BIND(val) (((unsigned char)(val)) >> 4)
#define ELF32_ST_TYPE(val) ((val)&0xf)
#define ELF32_ST_INFO(bind, type) (((bind) << 4) + ((type)&0xf))

#define ELF64_ST_BIND(val) ELF32_ST_BIND(val)
#define ELF64_ST_TYPE(val) ELF32_ST_TYPE(val)
#define ELF64_ST_INFO(bind, type) ELF32_ST_INFO((bind), (type))

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
void mod_update_info(env_t *ret);

void *elf_entry(void *module_bin);
void *elf_parse(elf64_header_t *head);

#endif // mod.h