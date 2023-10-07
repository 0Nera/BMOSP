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
struct limine_memmap_entry *memmap_entry;
uint8_t *bitmap;
uint64_t limit;
uint64_t usable = 0;
uint64_t available = 0;
uint64_t highest = 0;
uint64_t mmmap_count = 0;

lock_t lock = LOCK_INIT;

void init( ) {
	memmap_response = memmap_request.response;
	mmmap_count = memmap_response->entry_count;

	fb::printf("В карте памяти: %u записей.\n", memmap_response->entry_count);
}

void free(void *ptr, uint64_t frames) {}

void *malloc(uint64_t count) {
	return NULL;
}

void *calloc(uint64_t frames) {
	return (void *)0;
}

void *alloc(uint64_t size) {
	return (void *)0;
}
} // namespace mem