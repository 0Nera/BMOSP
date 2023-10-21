/**
 * lock.c
 * Функции блокировок
 *
 * Функционал блокировок и синхронизации
 *
 */

#include <fb.h>
#include <lock.h>
#include <stdint.h>
#include <tool.h>

int lock_swap(lock_t *lock) {
	return __sync_bool_compare_and_swap(&lock->lock, 0, 1);
}

void lock_acquire(lock_t *lock) {
	uint64_t count = 0;

	for (;;) {
		if (lock_swap(lock)) { break; }
		count++;
		if (count > 1000000) {
			fb_printf("%s блокировка зависла", lock->file);
			assert(0);
		}

		asm volatile("pause");
	}
}

void lock_release(lock_t *lock) {
	__sync_bool_compare_and_swap(&lock->lock, 1, 0);
}