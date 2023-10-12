#include <limine.h>
#include <stdarg.h>
#include <stdint.h>

namespace tool {

void memcpy(void *dest, void *src, uint64_t n) {
	char *d = (char *)dest;
	const char *s = (const char *)src;

	for (uint64_t i = 0; i < n; i++) { d[i] = s[i]; }
}

void *memset(void *ptr, uint8_t n, uint64_t size) {
	uint8_t *p = (uint8_t *)ptr;
	for (uint64_t i = 0; i < size; i++) { p[i] = n; }
	return ptr;
}

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
				// Преобразование целочисленного аргумента в строку и вывод
				// каждого символа
				if (arg < 0) {
					putc('-');
					arg = -arg;
				}
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[10]; // Предполагаем, что максимальное число из
					                 // 10 цифр
					int i = 0;

					while (arg > 0) {
						buffer[i++] = '0' + (arg % 10);
						arg /= 10;
					}

					while (i > 0) { putc(buffer[--i]); }
				}
			} else if (*format_string == 's') {
				const char *arg = va_arg(args, const char *);
				// Вывод каждого символа строки
				while (*arg != '\0') {
					putc(*arg);
					arg++;
				}
			} else if (*format_string == 'u') {
				unsigned int arg = va_arg(args, unsigned int);
				// Преобразование беззнакового целочисленного аргумента в строку
				// и вывод каждого символа
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[10]; // Предполагаем, что максимальное число из
					                 // 10 цифр
					int i = 0;

					while (arg > 0) {
						buffer[i++] = '0' + (arg % 10);
						arg /= 10;
					}

					while (i > 0) { putc(buffer[--i]); }
				}
			} else if (*format_string == 'x') {
				unsigned int arg = va_arg(args, unsigned int);
				// Преобразование беззнакового целочисленного аргумента в
				// шестнадцатеричную строку и вывод каждого символа
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[9]; // Предполагаем, что максимальное число из 8
					                // символов
					int i = 0;

					while (arg > 0) {
						int rem = arg % 16;
						if (rem < 10) {
							buffer[i++] = '0' + rem;
						} else {
							buffer[i++] = 'A' + (rem - 10);
						}
						arg /= 16;
					}

					while (i > 0) { putc(buffer[--i]); }
				}
			} else if (*format_string == 'c') {
				char arg = va_arg(args, int);
				// Вывод символа
				putc(arg);
			} else if (*format_string == 'o') {
				unsigned int arg = va_arg(args, unsigned int);
				// Преобразование беззнакового целочисленного аргумента в
				// восьмеричную строку и вывод каждого символа
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[12]; // Предполагаем, что максимальное число из
					                 // 11 символов
					int i = 0;

					while (arg > 0) {
						buffer[i++] = '0' + (arg % 8);
						arg /= 8;
					}

					while (i > 0) { putc(buffer[--i]); }
				}
			} else if (*format_string == 'b') {
				unsigned int arg = va_arg(args, unsigned int);
				// Преобразование беззнакового целочисленного аргумента в
				// двоичную строку и вывод каждого символа
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[33]; // Предполагаем, что максимальное число из
					                 // 32 символа
					int i = 0;

					while (arg > 0) {
						buffer[i++] = '0' + (arg % 2);
						arg /= 2;
					}

					while (i > 0) { putc(buffer[--i]); }
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

} // namespace tool