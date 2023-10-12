#include <stdint.h>

typedef struct {
	int reserved;
} framebuffer_t;

typedef struct {
	int reserved;
} sys_info_t;

typedef struct {
	int reserved;
} module_info_t;

typedef struct {
	int reserved;
} func_t;

typedef struct {
	uint8_t a[4];
	uint8_t b[4];
	uint8_t c[4];
	uint8_t d[4];
} uid_t;

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t second;
} time_t;

namespace sys {
void init( ) {}

framebuffer_t *alloc_framebuffer( ) {
	return (framebuffer_t *)0;
}

void free_framebuffer(framebuffer_t *frame) {}

void exit(int code) {}

int get_error( ) {
	return 0;
}

sys_info_t *get_info( ) {
	return (sys_info_t *)0;
}

module_info_t *get_module(uid_t module_id) {
	return (module_info_t *)0;
}

uid_t new_thread(func_t func) {
	return { 0, 0, 0, 0 };
}

int delete_thread(uid_t thread_id) {
	return 0;
}

time_t get_time( ) {
	return { 0, 0, 0, 0 };
}

void set_alarm(time_t time, func_t func) {}
} // namespace sys
