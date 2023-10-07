#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

namespace fb {

void init( );

void printf(char *str, ...);

void printf_at(size_t x, size_t y, char *str, ...);
} // namespace fb