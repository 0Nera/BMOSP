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

static uint64_t strlen(const char *str) {
	uint64_t length = 0;
	while (*str) {
		length++;
		str++;
	}
	return length;
}

static void memcpy(void *dest, void *src, uint64_t n) {
	char *d = (char *)dest;
	const char *s = (const char *)src;

	for (uint64_t i = 0; i < n; i++) { d[i] = s[i]; }
}

static void *memset(void *ptr, uint8_t n, uint64_t size) {
	uint8_t *p = (uint8_t *)ptr;
	for (uint64_t i = 0; i < size; i++) { p[i] = n; }
	return ptr;
}

static size_t strspn(const char *str, const char *accept) {
	size_t count = 0;
	const char *ptr = str;
	const char *acc;

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

static size_t strcspn(const char *str, const char *reject) {
	size_t count = 0;
	const char *ptr = str;
	const char *r;

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

static char *strtok(char *str, const char *delim) {
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

static char *strdup(const char *str) {
	size_t len = strlen(str) + 1;
	char *dup = alloc(len);

	if (dup != NULL) { memcpy(dup, str, len); }

	return dup;
}

static size_t count_chars(const char *str, char c) {
	size_t count = 0;

	while (*str) {
		if (*str == c) { count++; }
		str++;
	}

	return count;
}

#endif // modstd.h
