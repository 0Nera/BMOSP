/**
 * tool.c
 * Дополнительные функции
 *
 * Вспомогательные функции для работы с данными
 *
 */

#include <modstd.h>
#include <system.h>

uint64_t strlen(char *str) {
	uint64_t length = 0;
	while (*str) {
		length++;
		str++;
	}
	return length;
}

void strcpy(char *dest, char *src) {
	size_t i = 0;
	while (src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0'; // добавляем завершающий нулевой символ
}

size_t strspn(char *str, char *accept) {
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

size_t strcspn(char *str, char *reject) {
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

char *strtok(char *str, char *delim) {
	char *token = NULL;
	char *next_token = NULL;

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

size_t strtol(char *str, char **endptr, int64_t base) {
	size_t num = 0;
	int64_t sign = 1;

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
		int64_t digit;
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
		//*endptr = (char *)str; // Указатель на символ, следующий за числом
	}

	return num * sign;
}

char *strchr(char *str, char c) {
	// пройти по каждому символу строки
	while (*str != '\0') {
		// если символ найден, вернуть указатель на него
		if (*str == c) { return (char *)str; }
		str++; // переход к следующему символу
	}
	// символ не найден, вернуть NULL
	return NULL;
}

int strcmp(char *s1, char *s2) {
	while (*s1 == *s2) {
		if (*s1 == '\0') { return 0; }
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

char *trstr(char *str, char sym) {
	size_t left, size = strlen(str);
	for (left = 0x00U; left < size; left++)
		if (str[left] == sym) break;

	size++;
	left++;

	if (left < size)
		size -= left;
	else
		left = 0x00U;

	return str + left;
}

char *strdup(char *str) {
	size_t len = strlen(str) + 1;
	char *dup = alloc(len);

	if (dup != NULL) { memcpy(dup, str, len); }

	return dup;
}

size_t count_chars(char *str, char c) {
	size_t count = 0;
	size_t len = strlen(str);

	for (size_t i = 0; i < len; i++) {
		if (str[i] == c) { count++; }
	}

	return count;
}

uint64_t str_contains(char *str, char *substr) {
	uint64_t str_len = strlen(str);
	uint64_t substr_len = strlen(substr);

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

void memcpy(void *dest, void *src, size_t n) {
	char *d = (char *)dest;
	char *s = (char *)src;

	for (size_t i = 0; i < n; i++) { d[i] = s[i]; }
}

void *memset(void *ptr, uint8_t n, size_t size) {
	uint8_t *p = (uint8_t *)ptr;
	for (size_t i = 0; i < size; i++) { p[i] = n; }
	return ptr;
}

void *memmove(void *dest, void *src, size_t n) {
	char *d = (char *)dest;
	char *s = (char *)src;

	if (d > s) {
		// копирование с конца массива, чтобы предотвратить перекрытие
		for (size_t i = n; i > 0; --i) { d[i - 1] = s[i - 1]; }
	} else if (d < s) {
		// копирование с начала массива
		for (size_t i = 0; i < n; ++i) { d[i] = s[i]; }
	}

	return dest;
}