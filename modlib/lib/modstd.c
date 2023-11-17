/**
 * modstd.c
 * Стандартная библиотека модулей
 *
 * Вспомогательные функции для работы с модулями
 *
 */

#include <modstd.h>

int m_strcmp(const char *s1, const char *s2) {
	while (*s1 == *s2) {
		if (*s1 == '\0') { return 0; }
		s1++;
		s2++;
	}
	return *s1 - *s2;
}