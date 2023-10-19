#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

namespace fb {

void init( );
void print_buf(size_t x, size_t y, size_t h, size_t w, uint32_t *buf);
void printf(char *str, ...);

void printf_at(size_t x, size_t y, char *str, ...);
} // namespace fb