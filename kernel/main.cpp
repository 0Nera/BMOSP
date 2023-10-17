#include <fb.h>
#include <mem.h>
#include <tool.h>

void assert(int x) {
	if (!x) {
		fb::printf("assert(%d)\n", x);
		while (1) { asm volatile("hlt"); }
	}
}

void *malloc(int size) {}

void *realloc(void *pointer, int new_size) {}

void free(void *pointer) {}

void main( ) {}