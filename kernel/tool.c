#include <limine.h>
#include <stdarg.h>
#include <stdint.h>

void tool_memcpy(void *dest, void *src, uint64_t n) {
	char *d = (char *)dest;
	const char *s = (const char *)src;

	for (uint64_t i = 0; i < n; i++) { d[i] = s[i]; }
}

void *tool_memset(void *ptr, uint8_t n, uint64_t size) {
	uint8_t *p = (uint8_t *)ptr;
	for (uint64_t i = 0; i < size; i++) { p[i] = n; }
	return ptr;
}

uint64_t tool_strlen(const char *str) {
	uint64_t length = 0;
	while (*str) {
		length++;
		str++;
	}
	return length;
}

uint64_t tool_starts_with(const char *str, const char *prefix) {
	uint64_t str_len = tool_strlen(str);
	uint64_t prefix_len = tool_strlen(prefix);

	if (prefix_len > str_len) { return 0; }

	for (uint64_t i = 0; i < prefix_len; i++) {
		if (str[i] != prefix[i]) { return 0; }
	}

	return 1;
}

// Функция для форматированного вывода
void tool_format(void (*putc)(char c), const char *format_string,
                 va_list args) {
	while (*format_string != '\0') {
		if (*format_string == '%') {
			format_string++;
			if (*format_string == '\0') {
				break; // Неожиданный конец строки формата
			}
			if (*format_string == '%') {
				putc('%'); // Вывод одного символа '%'
			} else if (*format_string == 'd') {
				int64_t arg = va_arg(args, int64_t);
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
					int64_t i = 0;

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
				uint64_t arg = va_arg(args, uint64_t);
				// Преобразование беззнакового целочисленного аргумента в строку
				// и вывод каждого символа
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[32]; // Предполагаем, что максимальное число из
					                 // 10 цифр
					int64_t i = 0;

					while (arg > 0) {
						buffer[i++] = '0' + (arg % 10);
						arg /= 10;
					}

					while (i > 0) { putc(buffer[--i]); }
				}
			} else if (*format_string == 'x') {
				uint64_t arg = va_arg(args, uint64_t);
				// Преобразование беззнакового целочисленного аргумента в
				// шестнадцатеричную строку и вывод каждого символа
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[32]; // Предполагаем, что максимальное число из
					                 // 8 символов
					int64_t i = 0;

					while (arg > 0) {
						int64_t rem = arg % 16;
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
				uint64_t arg = va_arg(args, uint64_t);
				// Преобразование беззнакового целочисленного аргумента в
				// восьмеричную строку и вывод каждого символа
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[12]; // Предполагаем, что максимальное число из
					                 // 11 символов
					int64_t i = 0;

					while (arg > 0) {
						buffer[i++] = '0' + (arg % 8);
						arg /= 8;
					}

					while (i > 0) { putc(buffer[--i]); }
				}
			} else if (*format_string == 'b') {
				uint64_t arg = va_arg(args, uint64_t);
				// Преобразование беззнакового целочисленного аргумента в
				// двоичную строку и вывод каждого символа
				if (arg == 0) {
					putc('0');
				} else {
					char buffer[33]; // Предполагаем, что максимальное число из
					                 // 32 символа
					int64_t i = 0;

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
