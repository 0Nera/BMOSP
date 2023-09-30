#include <limine.h>
#include <stdarg.h>
#include <stdint.h>

namespace tool {
    // Функция для форматированного вывода
    void format(void (*putc)(char c), const char *format_string, va_list args) {
        while (*format_string != '\0') {
            if (*format_string == '%') {
                format_string++;
                if (*format_string == '\0') {
                    break; // Неожиданный конец строки формата
                }
                if (*format_string == '%') {
                    putc('%'); // Вывод одного символа '%'
                } else if (*format_string == 'd') {
                    int arg = va_arg(args, int);
                    // Преобразование целочисленного аргумента в строку и вывод каждого символа
                    if (arg < 0) {
                        putc('-');
                        arg = -arg;
                    }
                    if (arg == 0) {
                        putc('0');
                    } else {
                        char buffer[10]; // Предполагаем, что максимальное число из 10 цифр
                        int i = 0;

                        while (arg > 0) {
                            buffer[i++] = '0' + (arg % 10);
                            arg /= 10;
                        }

                        while (i > 0) {
                            putc(buffer[--i]);
                        }
                    }
                } else if (*format_string == 's') {
                    const char* arg = va_arg(args, const char*);
                    // Вывод каждого символа строки
                    while (*arg != '\0') {
                        putc(*arg);
                        arg++;
                    }
                } else {
                    // Неподдерживаемый спецификатор формата
                    putc('?');
                }
            } else {
                putc(*format_string);
            }

            format_string++;
        }
    }
}