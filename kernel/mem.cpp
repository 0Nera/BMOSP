#include <fb.h>
#include <limine.h>
#include <lock.h>
#include <tool.h>

#define BLOCK_SIZE 4096

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

namespace mem {

#define HHDM_OFFSET (hhdm_request.response->offset)

struct limine_memmap_response *memmap_response;

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

// Массив с описаниями типов памяти
char memory_types[8][82] = {
	"Доступно",      "Зарезервировано", "ACPI, можно освободить",
	"ACPI NVS",      "Плохая память",   "Загрузчик, можно освободить",
	"Ядро и модули", "Буфер кадра"
};

void free(void *ptr, uint64_t frames) {
	// Проход по фреймам памяти и очистка битов в битовой карте
	uint64_t frame = (uint64_t)ptr / BLOCK_SIZE;
	for (uint64_t i = frame; i < frames + frame; i++) { BIT_CLEAR(i); }
	bitmap_available++;
}

// Функция выделения памяти
void *alloc(uint64_t wanted_frames) {
	void *ptr;

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

			ptr = (void *)(BLOCK_SIZE * frame);
			return ptr;
		}
	}
	return NULL;
}

void *calloc(uint64_t frames) {
	void *ptr = alloc(frames);
	tool::memset(ptr + HHDM_OFFSET, 0, frames * BLOCK_SIZE);
	return ptr;
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

		// Пропускаем записи, не являющиеся доступными памятью
		if (!mmaps[i]->type == LIMINE_MEMMAP_USABLE) { continue; }

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
		if (!(mmaps[i]->type == LIMINE_MEMMAP_USABLE ||
		      mmaps[i]->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)) {
			continue;
		}

		for (uint64_t t = 0; t < mmaps[i]->length; t += BLOCK_SIZE) {
			free((void *)mmaps[i]->base + t, 1);
		}
	}

	fb::printf("%u МБ объем доступной памяти, %u МБ объем виртуальной памяти\n",
	           usable / 1024 / 1024, available / 1024 / 1024);

	fb::printf("%u / %u блоков доступно\n", bitmap_available, bitmap_limit);
	fb::printf("Размер битовой карты: %u\n", bitmap_size);
}

} // namespace mem