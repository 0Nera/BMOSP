/**
 * mem.c
 * Функции управления памятью
 *
 * Основной функционал менеджера памяти
 *
 */

#include <fb.h>
#include <limine.h>
#include <lock.h>
#include <log.h>
#include <mem.h>
#include <stdbool.h>
#include <tool.h>

static volatile struct limine_memmap_request memmap_request = { .id = LIMINE_MEMMAP_REQUEST,
	                                                            .revision = 0,
	                                                            .response = (struct limine_memmap_response *)0 };
static volatile struct limine_hhdm_request hhdm_request = { .id = LIMINE_HHDM_REQUEST,
	                                                        .revision = 0,
	                                                        .response = (struct limine_hhdm_response *)0 };

struct mem_entry {
	struct mem_entry *next;
	bool free;
	size_t size;
	uint8_t data[0];
};

typedef struct mem_entry mem_entry_t;

// Битовая карта для отслеживания занятых и свободных фреймов памяти
static uint8_t *bitmap;
// Объем доступных блоков
static uint64_t bitmap_available = 0;
// Объем блоков
static uint64_t bitmap_limit = 0;
// Верхняя граница доступной памяти
static uint64_t limit;
// Объем всего доступного физического адресного пространства
static uint64_t usable = 0;
// Объем доступной виртуальной памяти
static uint64_t available = 0;
// Наивысший адрес в available space
static uint64_t highest = 0;
// Количество записей в карте памяти
static uint64_t mmmap_count = 0;

static const char memory_types[8][82] = { "Доступно",      "Зарезервировано", "ACPI, можно освободить",
	                                      "ACPI NVS",      "Плохая память",   "Загрузчик, можно освободить",
	                                      "Ядро и модули", "Буфер кадра" };

static struct limine_memmap_response *memmap_response;

static mem_entry_t *first_node;

void mem_dump_memory( ) {
	mem_entry_t *curr = first_node;

	while (curr) {
		if (curr->next) {
			LOG("->0x%x | %u мегабайт | %s | 0x%x\n", &curr->data, (curr->size) / 1024 / 1024,
			    curr->free ? memory_types[0] : memory_types[1], curr->next);
		} else {
			LOG("->0x%x | %u мегабайт | %s | Это последний блок\n", &curr->data, (curr->size) / 1024 / 1024,
			    curr->free ? memory_types[0] : memory_types[1]);
		}
		curr = curr->next;
	}
}

void mem_get_stat( ) {
	size_t free_mem = 0;
	size_t used_mem = 0;

	struct mem_entry *current_entry = first_node;

	while (current_entry) {
		if (current_entry->free) {
			free_mem += current_entry->size;
		} else {
			used_mem += current_entry->size;
		}

		current_entry = current_entry->next;
	}

	LOG("Свободно: %u мегабайт\n", free_mem / 1024 / 1024);
	LOG("Занято: %u мегабайт\n", used_mem / 1024 / 1024);
}

void mem_check_dynamic_memory( ) {
	mem_entry_t *curr = first_node;
	uint64_t free_mem = 0;

	while (curr) {
		if (curr->free) { free_mem += curr->size; }
		curr = curr->next;
	}

	if (free_mem < 1024 * BLOCK_SIZE) {
		void *ptr = mem_frame_alloc(1024);
		if (ptr == NULL) {
			LOG("Память кончилась!\n");
			return;
		}
		mem_add_block(ptr, 1024 * BLOCK_SIZE);
		mem_merge_all_blocks( );
	}
}

void mem_frame_free(void *addr, uint64_t frames) {
	// Проход по фреймам памяти и очистка битов в битовой карте
	uint64_t frame = (uint64_t)addr / BLOCK_SIZE;
	for (uint64_t i = frame; i < frames + frame; i++) { BIT_CLEAR(i); }
	bitmap_available += frames;
}

// Функция выделения памяти
void *mem_frame_alloc(uint64_t wanted_frames) {
	void *addr;

	uint64_t available_frames = 0;
	for (uint64_t frame = 1; frame < limit; frame++) {
		if (!BIT_GET(frame)) {
			available_frames++;
		} else if (available_frames != wanted_frames) {
			available_frames = 0;
			continue;
		}
		if (available_frames == wanted_frames) {
			uint64_t i;
			for (i = 0; i < wanted_frames; i++) { BIT_SET(frame - i); }
			frame -= i - 1;

			addr = (void *)(BLOCK_SIZE * frame);
			bitmap_available -= wanted_frames;
			return addr;
		}
	}
	return NULL;
}

void *mem_frame_calloc(uint64_t frames) {
	void *addr = mem_frame_alloc(frames);
	tool_memset(addr + HHDM_OFFSET, 0, frames * BLOCK_SIZE);
	return addr;
}

static void merge_blocks(mem_entry_t *start) {
	if (!start->free) return;
	mem_entry_t *block = start;
	while (block->next && block->next->free) {
		block->size += block->next->size + sizeof(mem_entry_t);
		block->next = block->next->next;
	}
}

void mem_merge_all_blocks( ) {
	mem_entry_t *curr = first_node;

	while (curr) {
		merge_blocks(curr);
		curr = curr->next;
	}
}

void mem_add_block(void *addr, size_t size) {
	mem_entry_t *new_entry = (mem_entry_t *)addr;

	new_entry->size = size - sizeof(mem_entry_t);
	new_entry->free = true;

	if (first_node == NULL) {
		first_node = new_entry;
		new_entry->next = NULL;
	} else {
		mem_entry_t *curr = first_node;
		while (curr->next != NULL) { curr = curr->next; }

		curr->next = new_entry;
		new_entry->next = NULL;
	}
}

static void alloc_init(void *address, size_t length) {
	first_node = (mem_entry_t *)address;

	first_node->size = length - sizeof(mem_entry_t);
	first_node->free = true;
	first_node->next = NULL;
}

static void *alloc_align(size_t size, size_t alignment) {
	mem_entry_t *curr = first_node;

	while (curr) {
		if (curr->free) {
			void *addr = curr->data + alignment - 1;
			addr -= (uintptr_t)addr % alignment + sizeof(mem_entry_t);
			mem_entry_t *second = (mem_entry_t *)addr;
			if (curr->size >= (second->data - curr->data + size)) {
				mem_entry_t *third = (mem_entry_t *)(second->data + size);

				third->size = curr->size - (third->data - curr->data);
				third->next = curr->next;
				third->free = 1;

				second->size = size;
				second->next = third;
				second->free = 0;

				if (curr != second) {
					curr->next = second;
					curr->size = (uintptr_t)second - (uintptr_t)curr->data;
					curr->free = 1;
				}

				return second->data;
			}
		}

		curr = curr->next;
	}
	return NULL;
}

void *mem_alloc(size_t size) {
	mem_check_dynamic_memory( );
	void *data = alloc_align(size, 1);
	return data;
}

void mem_free(void *addr) {
	mem_entry_t *curr = first_node, *prev = NULL;
	while (curr != NULL) {
		if (curr->data == addr) {
			curr->free = 1;
			merge_blocks(prev ? prev : curr);
			mem_merge_all_blocks( );
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

void *mem_realloc(void *addr, size_t size) {
	if (size == 0) {
		mem_free(addr);
		return NULL;
	}

	if (addr == NULL) { return mem_alloc(size); }

	void *new_addr = mem_alloc(size);

	if (new_addr == NULL) { return NULL; }

	tool_memcpy(new_addr, addr, size);
	mem_free(addr);

	return new_addr;
}

// Инициализация менеджера памяти
void mem_init( ) {
	// Получение информации о доступной памяти из Limine bootloader
	memmap_response = memmap_request.response;
	mmmap_count = memmap_response->entry_count;
	struct limine_memmap_entry **mmaps = memmap_response->entries;

	LOG("Записей в карте памяти: %u\n", memmap_response->entry_count);

	// Обработка каждой записи в карте памяти
	for (uint64_t i = 0; i < mmmap_count; i++) {
		available += mmaps[i]->length;

		// LOG("\t%d: 0x%x\tдлина: 0x%x\tтип: %s\n", i + 1, mmaps[i]->base, mmaps[i]->length,
		//     memory_types[mmaps[i]->type]);
		if (mmaps[i]->type == LIMINE_MEMMAP_FRAMEBUFFER) {
			LOG("На видеопамять BIOS/UEFI выделено: %u мегабайт + %u "
			    "килобайт\n",
			    mmaps[i]->length / 1024 / 1024, (mmaps[i]->length / 1024) % 1024);
		}
		if (!(mmaps[i]->type == LIMINE_MEMMAP_USABLE)) { continue; }

		usable += mmaps[i]->length;
		uint64_t top = mmaps[i]->base + mmaps[i]->length;
		if (top > highest) highest = top;
	}

	limit = highest / BLOCK_SIZE;
	uint64_t bitmap_size = ALIGN_UP(highest / BLOCK_SIZE / 8, BLOCK_SIZE);

	// Находим доступное место для битовой карты и устанавливаем ее
	for (uint64_t i = 0; i < mmmap_count; i++) {
		if (!mmaps[i]->type == LIMINE_MEMMAP_USABLE) continue;

		if (mmaps[i]->length >= bitmap_size) {
			bitmap = (uint8_t *)mmaps[i]->base;
			tool_memset(bitmap, 0xFF, bitmap_size);
			mmaps[i]->length -= bitmap_size;
			mmaps[i]->base += bitmap_size;
			available -= bitmap_size;
			break;
		}
	}

	// Освобождаем все доступные фреймы памяти
	for (uint64_t i = 0; i < mmmap_count; i++) {
		for (uint64_t t = 0; t < mmaps[i]->length; t += BLOCK_SIZE) { bitmap_limit++; }

		if (!(mmaps[i]->type == LIMINE_MEMMAP_USABLE)) { continue; }

		for (uint64_t t = 0; t < mmaps[i]->length; t += BLOCK_SIZE) { mem_frame_free((void *)mmaps[i]->base + t, 1); }
	}

	LOG("%u / %u блоков доступно\n", bitmap_available, bitmap_limit);
	LOG("Размер битовой карты: %u\n", bitmap_size);

	alloc_init(mem_frame_alloc(1024), 1024 * BLOCK_SIZE);
	LOG("%u мегабайт выделено в динамичную память\n", (256 * 32 * BLOCK_SIZE + BLOCK_SIZE) / 1024 / 1024);

	// Выделяем по 4 мегабайта в аллокатор динамичной памяти
	for (uint64_t i = 0; i < 64; i += 8) { mem_add_block(mem_frame_alloc(1024), 1024 * BLOCK_SIZE); }

	mem_merge_all_blocks( );
	mem_dump_memory( );

	LOG("%u МБ объем доступной памяти, %u МБ объем виртуальной памяти\n", (bitmap_available * BLOCK_SIZE) / 1024 / 1024,
	    available / 1024 / 1024);

	LOG("%u / %u блоков доступно\n", bitmap_available, bitmap_limit);
}