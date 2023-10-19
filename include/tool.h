#include <stdarg.h>

#define abs(x) ((x) < 0 ? -(x) : (x))

#define assert(check)                                                          \
	({                                                                         \
		if (!(check)) {                                                        \
			fb::printf("\nassert() failed in %s() (%s:%d)\n", __func__,        \
			           __FILE__, __LINE__);                                    \
			for (;;) asm volatile("hlt");                                      \
		}                                                                      \
	})

#define ALIGN_UP(NUM, ALIGN) (((NUM) + ALIGN - 1) & ~(ALIGN - 1))
#define ALIGN_DOWN(NUM, ALIGN) ((NUM) & ~(ALIGN - 1))
#define CONTAINER_OF(PTR, TYPE, MEMBER)                                        \
	((TYPE *)((void *)PTR - OFFSET_OF(TYPE, MEMBER)))

#define BIT_SET(BIT) (bitmap[(BIT) / 8] |= (1 << ((BIT) % 8)))
#define BIT_CLEAR(BIT) (bitmap[(BIT) / 8] &= ~(1 << ((BIT) % 8)))
#define BIT_GET(BIT) ((bitmap[(BIT) / 8] >> ((BIT) % 8)) & 1)

static inline void pause( ) {
	for (uint64_t i = 0; i < 1024; i++) {
		for (uint64_t j = 0; j < 1024; j++) {
			for (uint64_t q = 0; q < 8; q++) { asm volatile("pause"); }
		}
	}
}

namespace tool {
void memcpy(void *dest, void *src, uint64_t n);
void *memset(void *ptr, uint8_t n, uint64_t size);
uint64_t strlen(const char *str);
uint64_t starts_with(const char *str, const char *prefix);
void format(void (*putc)(char c), const char *format_string, va_list args);
} // namespace tool