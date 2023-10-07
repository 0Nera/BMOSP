#include <fb.h>
#include <lock.h>
#include <stdint.h>
#include <tool.h>

namespace lock {
int swap(lock_t *lock) {
	return __sync_bool_compare_and_swap(&lock->lock, 0, 1);
}

void acquire(lock_t *lock) {
	uint64_t count = 0;

	for (;;) {
		if (swap(lock)) { break; }
		count++;
		if (count > 1000000) {
			fb::printf("%s deadlock", lock->file);
			assert(0);
		}

		asm volatile("pause");
	}
}

void release(lock_t *lock) {
	__sync_bool_compare_and_swap(&lock->lock, 1, 0);
}
} // namespace lock