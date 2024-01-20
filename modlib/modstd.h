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

uint64_t strlen(char *str);
void strcpy(char *dest, char *src);
size_t strspn(char *str, char *accept);
size_t strcspn(char *str, char *reject);
char *strtok(char *str, char *delim);
size_t strtol(char *str, char **endptr, int64_t base);
char *strchr(char *str, char c);
int strcmp(const char *s1, const char *s2);
char *trstr(char *str, char sym);
char *strdup(char *str);
size_t count_chars(char *str, char c);
void memcpy(void *dest, void *src, size_t n);
void *memset(void *ptr, uint8_t n, size_t size);
void *memmove(void *dest, void *src, size_t n);

#endif // modstd.h
