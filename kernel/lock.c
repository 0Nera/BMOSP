/**
 * lock.c
 * Функции блокировок
 *
 * Функционал блокировок и синхронизации
 *
 */

#include <fb.h>
#include <lock.h>
#include <log.h>
#include <stdint.h>
#include <tool.h>

// Если не заблокировано - блокируем
int lock_swap(lock_t *lock) {
	return __sync_bool_compare_and_swap(&lock->lock, 0, 1);
}

// Запрос блокировки ресурса
void lock_acquire(lock_t *lock) {
	uint64_t count = 0;

	for (;;) {
		if (lock_swap(lock)) { break; }
		count++;
		if (count > 1000000) {
			LOG("%s блокировка зависла", lock->file);
			assert(0);
		}

		asm volatile("pause");
	}
}

// Запрос разблокировки ресурса
void lock_release(lock_t *lock) {
	__sync_bool_compare_and_swap(&lock->lock, 1, 0);
}