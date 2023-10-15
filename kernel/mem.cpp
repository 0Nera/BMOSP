#include <fb.h>
#include <limine.h>
#include <lock.h>
#include <tool.h>

#define BLOCK_SIZE 4096
#define HHDM_OFFSET (hhdm_request.response->offset)

static volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0,
	.response = (struct limine_memmap_response *)0
};
static volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0,
	.response = (struct limine_hhdm_response *)0
};

struct mem_entry {
	struct mem_entry *next;
	bool free;
	size_t size;
	uint8_t data[0];
};
typedef struct mem_entry mem_entry_t;

// Битовая карта для отслеживания занятых и свободных фреймов памяти
uint8_t *bitmap;
// Объем доступных блоков
uint64_t bitmap_available = 0;
// Объем блоков
uint64_t bitmap_limit = 0;
// Верхняя граница доступной памяти
uint64_t limit;
// Объем всего доступного физического адресного пространства
uint64_t usable = 0;
// Объем доступной виртуальной памяти
uint64_t available = 0;
// Наивысший адрес в available space
uint64_t highest = 0;
// Количество записей в карте памяти
uint64_t mmmap_count = 0;

const char memory_types[8][82] = {
	"Доступно",      "Зарезервировано", "ACPI, можно освободить",
	"ACPI NVS",      "Плохая память",   "Загрузчик, можно освободить",
	"Ядро и модули", "Буфер кадра"
};

struct limine_memmap_response *memmap_response;

static mem_entry_t *first_node;

namespace mem {

void frame_free(void *addr, uint64_t frames) {
	// Проход по фреймам памяти и очистка битов в битовой карте
	uint64_t frame = (uint64_t)addr / BLOCK_SIZE;
	for (uint64_t i = frame; i < frames + frame; i++) { BIT_CLEAR(i); }
	bitmap_available += frames;
}

// Функция выделения памяти
void *frame_alloc(uint64_t wanted_frames) {
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

void *frame_calloc(uint64_t frames) {
	void *addr = frame_alloc(frames);
	tool::memset(addr + HHDM_OFFSET, 0, frames * BLOCK_SIZE);
	return addr;
}

void add_block(void *addr, size_t size) {
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

void alloc_init(void *address, size_t length) {
	first_node = (mem_entry_t *)address;

	first_node->size = length - sizeof(mem_entry_t);
	first_node->free = true;
	first_node->next = NULL;
}

void merge_blocks(mem_entry_t *start) {
	if (!start->free) return;
	mem_entry_t *block = start;
	while (block->next && block->next->free) {
		block->size += block->next->size + sizeof(mem_entry_t);
		block->next = block->next->next;
	}
}

void *alloc_align(size_t size, size_t alignment) {
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

void *alloc(size_t size) {
	return alloc_align(size, 1);
}

void free(void *addr) {
	mem_entry_t *curr = first_node, *prev = NULL;
	while (curr != NULL) {
		if (curr->data == addr) {
			curr->free = 1;
			merge_blocks(prev ? prev : curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

void *realloc(void *addr, size_t size) {
	if (size == 0) {
		free(addr);
		return NULL;
	}

	if (addr == NULL) { return alloc(size); }

	void *new_addr = alloc(size);

	if (new_addr == NULL) { return NULL; }

	tool::memcpy(new_addr, addr, size);
	free(addr);

	return new_addr;
}

// Инициализация менеджера памяти
void init( ) {
	// Получение информации о доступной памяти из Limine bootloader
	memmap_response = memmap_request.response;
	mmmap_count = memmap_response->entry_count;
	struct limine_memmap_entry **mmaps = memmap_response->entries;

	fb::printf("В карте памяти: %u записей.\n", memmap_response->entry_count);

	// Обработка каждой записи в карте памяти
	for (int i = 0; i < mmmap_count; i++) {
		available += mmaps[i]->length;

		// fb::printf("\t%d: 0x%x\tдлина: 0x%x\tтип: %s\n", i + 1,
		// mmaps[i]->base, mmaps[i]->length, memory_types[mmaps[i]->type]);
		if (mmaps[i]->type == LIMINE_MEMMAP_FRAMEBUFFER) {
			fb::printf("На видеопамять BIOS/UEFI выделено: %u мегабайт + %u "
			           "килобайт\n",
			           mmaps[i]->length / 1024 / 1024,
			           (mmaps[i]->length / 1024) % 1024);
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
			tool::memset(bitmap, 0xFF, bitmap_size);
			mmaps[i]->length -= bitmap_size;
			mmaps[i]->base += bitmap_size;
			available -= bitmap_size;
			break;
		}
	}

	// Освобождаем все доступные фреймы памяти
	for (uint64_t i = 0; i < mmmap_count; i++) {
		for (uint64_t t = 0; t < mmaps[i]->length; t += BLOCK_SIZE) {
			bitmap_limit++;
		}

		if (!(mmaps[i]->type == LIMINE_MEMMAP_USABLE)) { continue; }

		for (uint64_t t = 0; t < mmaps[i]->length; t += BLOCK_SIZE) {
			frame_free((void *)mmaps[i]->base + t, 1);
		}
	}

	fb::printf("%u / %u блоков доступно\n", bitmap_available, bitmap_limit);
	fb::printf("Размер битовой карты: %u\n", bitmap_size);
	for (uint64_t i = 0; i < 256; i++) {
		alloc_init(frame_alloc(100), 100 * BLOCK_SIZE);
	}
	fb::printf("%u мегабайт выделено в динамичную память\n",
	           (256 * 100 * BLOCK_SIZE) / 1024 / 1024);
	fb::printf("%u МБ объем доступной памяти, %u МБ объем виртуальной памяти\n",
	           (bitmap_available * BLOCK_SIZE) / 1024 / 1024,
	           available / 1024 / 1024);

	fb::printf("%u / %u блоков доступно\n", bitmap_available, bitmap_limit);
}

} // namespace mem