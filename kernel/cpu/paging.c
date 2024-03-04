/**
 * paging.c
 * Страничная адресация
 *
 * Функционал для работы со страницами памяти
 *
 */

#include <arch.h>
#include <log.h>

typedef struct {
	uint64_t present : 1;
	uint64_t rw : 1;
	uint64_t user : 1;
	uint64_t pwt : 1;
	uint64_t pcd : 1;
	uint64_t accessed : 1;
	uint64_t dirty : 1;
	uint64_t pat : 1;
	uint64_t global : 1;
	uint64_t ignored : 3;
	uint64_t addr : 40;
	uint64_t reserved : 11;
	uint64_t no_execute : 1;
} page_table_entry_t;

// Получение адреса CR3
static inline uint64_t get_cr3( ) {
	uint64_t cr3;
	asm volatile("mov %%cr3, %0" : "=r"(cr3));
	return cr3;
}

// Вывод флагов
void print_flags(page_table_entry_t entry) {
	LOG("\tФлаги: [%c%c%c%c%c%c%c]\n", entry.present ? 'P' : '-', entry.rw ? 'W' : 'R', entry.user ? 'U' : '-',
	    entry.pwt ? 'T' : '-', entry.pcd ? 'D' : '-', entry.accessed ? 'A' : '-', entry.no_execute ? 'X' : 'E');
}

// Вывод структуры таблицы страниц, начиная с CR3
void print_page_structure(uint64_t cr3) {
	page_table_entry_t* pml4 = (page_table_entry_t*)(cr3 & ~0xFFF); // Получаем адрес PML4
	LOG("PML4 Address: 0x%x\n", pml4);
	for (uint64_t i = 0; i < 512; i++) {
		if (pml4[i].present) {
			LOG("PML4[%d] - present\n", i);

			print_flags(pml4[i]);
			LOG("Адрес: 0x%x\n", pml4[i].addr);
		}
	}
}

void paging_init( ) {
	LOG("Paging...\n");
	print_page_structure(get_cr3( ));
}