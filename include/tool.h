/**
 * tool.h
 * Заголовок содержащий определения вспомогательных функций
 *
 * Данный заголовочный файл содержит определения которые используются для
 * упрощения написания кода
 *
 */

#ifndef TOOL_H
#define TOOL_H

#include <stdarg.h>
#include <stdint.h>

#define UNUSED(x) (void)(x)

#define abs(x) ((x) < 0 ? -(x) : (x))

#define assert(check)                                                                                                  \
	do {                                                                                                               \
		if (!(check)) {                                                                                                \
			LOG("\nassert() ошибка в %s() (%s:%d)\n", __func__, __FILE__, __LINE__);                                   \
			for (;;) asm volatile("hlt");                                                                              \
		}                                                                                                              \
	} while (0)

#define ALIGN_UP(NUM, ALIGN) (((NUM) + ALIGN - 1) & ~(ALIGN - 1))
#define ALIGN_DOWN(NUM, ALIGN) ((NUM) & ~(ALIGN - 1))
#define CONTAINER_OF(PTR, TYPE, MEMBER) ((TYPE *)((void *)PTR - offsetof(TYPE, MEMBER)))

#define BIT_SET(BIT) (bitmap[(BIT) / 8] |= (1 << ((BIT) % 8)))
#define BIT_CLEAR(BIT) (bitmap[(BIT) / 8] &= ~(1 << ((BIT) % 8)))
#define BIT_GET(BIT) ((bitmap[(BIT) / 8] >> ((BIT) % 8)) & 1)

static inline void pause( ) {
	for (uint64_t i = 0; i < 1024; i++) {
		for (uint64_t j = 0; j < 4; j++) { asm volatile("pause"); }
	}
}

void tool_memcpy(void *dest, void *src, uint64_t n);
void *tool_memset(void *ptr, uint8_t n, uint64_t size);
void tool_memmove(void *dest, void *src, uint64_t n);
uint64_t tool_strlen(const char *str);
void tool_strcpy(char *dest, char *src);
int tool_strcmp(const char *s1, const char *s2);
uint64_t tool_starts_with(const char *str, const char *prefix);
uint64_t tool_str_contains(const char *str, const char *substr);
void tool_format(void (*putc)(char c), const char *format_string, va_list args);

#endif // tool.h
