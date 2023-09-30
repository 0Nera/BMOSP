#include <stdarg.h>


namespace tool {
    void format(void (*putc)(char c), const char *format_string, va_list args);
}