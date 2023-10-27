/**
 * lock.h
 * Заголовок с функциями блокировок
 *
 * Данный заголовочный файл содержит определения которые используются для работы
 * с блокировками ресурсов
 *
 */

#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

typedef struct {
	int lock;
	const char* file;
} lock_t;

#define LOCK_INIT                                                              \
	(lock_t) { 0, __FILE__ }

int lock_swap(lock_t* lock);
void lock_acquire(lock_t* lock);
void lock_release(lock_t* lock);

#endif // lock.h
