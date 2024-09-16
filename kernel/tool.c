/**
 * tool.c
 * Функции для упрощения разработки
 *
 * Данный файл содержит реализацию функций для упрощения написания кода
 *
 */

#include <stdarg.h>
#include <stdbool.h>
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

void tool_memmove(void *dest, void *src, uint64_t n) {
	unsigned char *cdest = (unsigned char *)dest;
	unsigned char *csrc = (unsigned char *)src;

	if (cdest < csrc) {
		for (uint64_t i = 0; i < n; i++) { cdest[i] = csrc[i]; }
	} else {
		for (uint64_t i = n; i > 0; i--) { cdest[i - 1] = csrc[i - 1]; }
	}
}

uint64_t tool_strlen(const char *str) {
	uint64_t length = 0;
	while (*str) {
		length++;
		str++;
	}
	return length;
}

void tool_strcpy(char *dest, char *src) {
	uint64_t i = 0;
	while (src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

void tool_strcat(char *str1, const char *str2) {
	while (*str1 != '\0') { str1++; }

	while (*str2 != '\0') {
		*str1 = *str2;
		str1++;
		str2++;
	}

	*str1 = '\0';
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

uint64_t tool_str_contains(const char *str, const char *substr) {
	uint64_t str_len = tool_strlen(str);
	uint64_t substr_len = tool_strlen(substr);

	if (substr_len > str_len) { return 0; }

	for (uint64_t i = 0; i <= str_len - substr_len; i++) {
		uint64_t j;
		for (j = 0; j < substr_len; j++) {
			if (str[i + j] != substr[j]) { break; }
		}

		if (j == substr_len) { return 1; }
	}

	return 0;
}

// Функция для переворачивания строки
void tool_reverse_str(char *str) {
	int len = 0;
	while (str[len] != '\0') { len++; }

	int start = 0;
	int end = len - 1;

	while (start < end) {
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}
}

int tool_strcmp(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

// Преобразование целого числа "i" в системе счисления "base" в строку "buf"
void tool_int_to_str(int64_t i, uint8_t base, char *buf) {
	bool negative = false;

	// Проверяем, является ли число отрицательным
	if (i < 0) {
		negative = true;
		i *= -1; // Если да, то умножаем на -1, чтобы сделать его положительным
	}

	int64_t index = 0;
	// Деление с остатком для преобразования числа в нужную систему счисления
	do {
		int64_t remainder = i % base;
		// Преобразовываем остаток в символ и добавляем его в строку
		buf[index++] =
		    (remainder > 9) ? (remainder - 10) + 'A' : remainder + '0'; // Если остаток > 9, добавляем заглавную букву А
		i /= base;
	} while (i > 0);

	// Если число было отрицательным, добавляем знак минуса в конец строки
	if (negative) { buf[index++] = '-'; }

	// Добавляем нулевой символ в конец строки, чтобы завершить ее
	buf[index] = '\0';

	// Переворачиваем строку, чтобы цифры были в правильном порядке
	tool_reverse_str(buf);
}

// Преобразование неотрицательного целого числа "i" в системе счисления "base" в
// строку "buf"
void tool_uint_to_str(uint64_t i, uint8_t base, char *buf) {
	uint64_t index = 0;
	// Деление с остатком для преобразования числа в нужную систему счисления
	do {
		uint64_t remainder = i % base;
		// Преобразовываем остаток в символ и добавляем его в строку
		buf[index++] =
		    (remainder > 9) ? (remainder - 10) + 'A' : remainder + '0'; // Если остаток > 9, добавляем заглавную букву А
		i /= base;
	} while (i > 0);

	// Добавляем нулевой символ в конец строки, чтобы завершить ее
	buf[index] = '\0';

	// Переворачиваем строку, чтобы цифры были в правильном порядке
	tool_reverse_str(buf);
}

void tool_uint_to_wstr(uint64_t i, uint8_t base, char *buf, uint64_t width) {
	uint64_t index = 0;
	// Деление с остатком для преобразования числа в нужную систему счисления
	do {
		uint64_t remainder = i % base;
		// Преобразовываем остаток в символ и добавляем его в строку
		buf[index++] =
		    (remainder > 9) ? (remainder - 10) + 'A' : remainder + '0'; // Если остаток > 9, добавляем заглавную букву А
		i /= base;
	} while (i > 0);

	while (index < width) {
		buf[index++] = ' '; // Добавляем пробелы слева до заданной ширины
	}

	// Добавляем нулевой символ в конец строки, чтобы завершить ее
	buf[index] = '\0';

	// Переворачиваем строку, чтобы цифры были в правильном порядке
	tool_reverse_str(buf);
}

int is_digit(char c) {
	if (c >= '0' && c <= '9') { return 1; }
	return 0;
}

int64_t char_to_digit(char c) {
	if (is_digit(c)) { return (int64_t)(c - '0'); }
	return -1;
}

uint64_t tool_uint_pow(uint64_t d, uint64_t n) {
	uint64_t result = 1;
	for (uint64_t i = 0; i < n; i++) { result *= d; }
	return result;
}

// Функция для форматированного вывода
void tool_format(void (*putc)(char c), const char *format_string, va_list args) {
	while (*format_string != '\0') {
		if (*format_string == '%') {
			char buf[48];
			uint64_t point = 0;
			char *arg_s;
			int64_t arg_d = 0;
			uint64_t arg_u = 0;
			int64_t width = 0;

			format_string++;

			while (is_digit(*format_string)) {
				arg_u += char_to_digit(*format_string);

				if (is_digit(*(format_string + 1))) { arg_u *= 10; }

				format_string++;
			}

			width = arg_u;

			if (*format_string == '\0') {
				break; // Неожиданный конец строки формата
			}

			switch (*format_string) {
				case '%': putc('%'); break;
				case 'c': putc(va_arg(args, int)); break;
				case 's':
					arg_s = va_arg(args, char *);

					if (width) { width -= tool_strlen(arg_s); }

					// Вывод каждого символа строки
					while (*arg_s != '\0') {
						putc(*arg_s);
						arg_s++;
					}
					while (width > 0) {
						putc(' ');
						width--;
					}
					break;
				case 'd':
					arg_d = va_arg(args, int64_t);
					tool_int_to_str(arg_d, 10, buf);
					while (buf[point] != '\0') {
						putc(buf[point]);
						point++;
					}
					break;
				case 'u':
					arg_u = va_arg(args, uint64_t);
					tool_uint_to_wstr(arg_u, 10, buf, width);
					while (buf[point] != '\0') {
						putc(buf[point]);
						point++;
					}
					break;
				case 'x':
					arg_u = va_arg(args, uint64_t);
					tool_uint_to_wstr(arg_u, 16, buf, width);
					while (buf[point] != '\0') {
						putc(buf[point]);
						point++;
					}
					break;
				default: putc('?'); break;
			}
		} else {
			putc(*format_string);
		}

		format_string++;
	}
}
