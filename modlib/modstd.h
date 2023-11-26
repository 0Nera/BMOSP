/**
 * modstd.h
 * Стандартная библиотека для модулей
 *
 * Заголовочный файл содержащий определения стандартной библиотеки модулей
 *
 */

#include <types.h>

#ifndef MODSTD_H
#define MODSTD_H

static uint64_t strlen(char *str) {
	uint64_t length = 0;
	while (*str) {
		length++;
		str++;
	}
	return length;
}

static void memcpy(void *dest, void *src, uint64_t n) {
	char *d = (char *)dest;
	char *s = (char *)src;

	for (uint64_t i = 0; i < n; i++) { d[i] = s[i]; }
}

static void *memset(void *ptr, uint8_t n, uint64_t size) {
	uint8_t *p = (uint8_t *)ptr;
	for (uint64_t i = 0; i < size; i++) { p[i] = n; }
	return ptr;
}

static size_t strspn(char *str, char *accept) {
	size_t count = 0;
	char *ptr = str;
	char *acc;

	while (*ptr) {
		acc = accept;
		while (*acc) {
			if (*ptr == *acc) {
				count++;
				break;
			}
			acc++;
		}
		if (*acc == '\0') break;
		ptr++;
	}

	return count;
}

static size_t strcspn(char *str, char *reject) {
	size_t count = 0;
	char *ptr = str;
	char *r;

	while (*ptr) {
		r = reject;
		while (*r) {
			if (*ptr == *r) { return count; }
			r++;
		}
		count++;
		ptr++;
	}

	return count;
}

static char *strtok(char *str, char *delim) {
	static char *token = NULL;
	static char *next_token = NULL;

	if (str != NULL) {
		token = str;
	} else {
		token = next_token;
	}

	if (token == NULL) { return NULL; }

	token += strspn(token, delim);

	if (*token == '\0') {
		next_token = NULL;
		return NULL;
	}

	next_token = token + strcspn(token, delim);

	if (*next_token != '\0') {
		*next_token = '\0';
		next_token++;
	} else {
		next_token = NULL;
	}

	return token;
}

static long int strtol(char *str, char **endptr, int base) {
	long int num = 0;
	int sign = 1;

	// Пропускаем пробелы в начале строки
	while (*str == ' ') { str++; }

	// Проверяем знак числа
	if (*str == '-') {
		sign = -1;
		str++;
	} else if (*str == '+') {
		str++;
	}

	// Проверяем основание системы счисления
	if (base == 0) {
		// Автоопределение основания
		if (*str == '0') {
			str++;
			if (*str == 'x' || *str == 'X') {
				base = 16; // Шестнадцатеричная система счисления
				str++;
			} else {
				base = 8; // Восьмеричная система счисления
			}
		} else {
			base = 10; // Десятичная система счисления
		}
	}

	// Преобразование строки в число
	while (*str != '\0') {
		int digit;
		if (*str >= '0' && *str <= '9') {
			digit = *str - '0';
		} else if (*str >= 'A' && *str <= 'Z') {
			digit = *str - 'A' + 10;
		} else if (*str >= 'a' && *str <= 'z') {
			digit = *str - 'a' + 10;
		} else {
			break; // Некорректный символ - прерываем преобразование
		}

		if (digit >= base) {
			break; // Некорректная цифра - прерываем преобразование
		}

		num = num * base + digit;
		str++;
	}

	if (endptr != NULL) {
		*endptr = (char *)str; // Указатель на символ, следующий за числом
	}

	return num * sign;
}

static char *strchr(char *str, char c) {
	// пройти по каждому символу строки
	while (*str != '\0') {
		// если символ найден, вернуть указатель на него
		if (*str == c) { return (char *)str; }
		str++; // переход к следующему символу
	}
	// символ не найден, вернуть NULL
	return NULL;
}

static void *memmove(void *dest, void *src, size_t n) {
	char *d = (char *)dest;
	const char *s = (const char *)src;

	if (d > s) {
		// копирование с конца массива, чтобы предотвратить перекрытие
		for (size_t i = n; i > 0; --i) { d[i - 1] = s[i - 1]; }
	} else if (d < s) {
		// копирование с начала массива
		for (size_t i = 0; i < n; ++i) { d[i] = s[i]; }
	}

	return dest;
}

static char *trstr(char *str, char sym) {
	size_t left, size = strlen(str);
	for (left = 0x00U; left < size; left++)
		if (str[left] == sym) break;

	size++;
	left++;

	if (left < size)
		size -= left;
	else
		left = 0x00U;

	char *res = alloc(size);
	memcpy(res, str + left, size);
	return res;
}

static char *strdup(char *str) {
	size_t len = strlen(str) + 1;
	char *dup = alloc(len);

	if (dup != NULL) { memcpy(dup, str, len); }

	return dup;
}

static size_t count_chars(char *str, char c) {
	size_t count = 0;
	size_t len = strlen(str);

	for (size_t i = 0; i < len; i++) {
		if (str[i] == c) { count++; }
	}

	return count;
}

#endif // modstd.h
