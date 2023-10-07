#include <stdint.h>

typedef struct {
	int lock;
	const char* file;
} lock_t;

#define LOCK_INIT                                                              \
	(lock_t) { 0, __FILE__ }

namespace lock {
int swap(lock_t* lock);
void acquire(lock_t* lock);
void release(lock_t* lock);
} // namespace lock
