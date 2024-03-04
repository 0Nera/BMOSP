/**
 * types.h
 * Системные вызовы
 *
 * Заголовочный файл содержащий заготовку для работы с системными вызовами
 *
 */

#ifndef TYPES_H
#define TYPES_H

#define NULL ((void *)0)
#define bool _Bool
#define false 0
#define true 1

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef uint64_t size_t;

typedef struct {
	uint32_t *address;
	uint64_t width;
	uint64_t height;
	uint64_t pitch;
	uint16_t bpp;
	int reserved;
} framebuffer_t;

typedef struct {
	int reserved;
} sys_info_t;

typedef struct {
	char *name;
	void *addr;
} module_func_t;

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
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hours;
	uint8_t minutes;
	uint8_t second;
} time_t;

struct frame {
	uint64_t rbp;
	uint64_t rbx;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rax;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t int_number;
	uint64_t err;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} __attribute__((packed));

typedef void (*int_entry_t)(struct frame *state);

typedef struct {
	char *name;
	char *message;
	uint64_t type;
	uint64_t data_size;
	void *data;
	int64_t err_code;
	uint64_t module_id;
	uint8_t irq;
	int_entry_t irq_handler;
	void *(*get_func)(uint64_t id);
	void (*after_init)( );
} __attribute__((packed)) module_info_t;

typedef struct {
	uint64_t offset;
	void (*log_printf)(char *str, ...); // Временная функция
	framebuffer_t (*alloc_framebuffer)( );
	void (*free_framebuffer)(framebuffer_t *frame);
	void *(*alloc)(uint64_t size);
	void (*free)(void *ptr);
	void (*exit)(int code);
	int (*get_error)( );
	sys_info_t *(*get_info)( );
	module_info_t *(*get_module)(char *module_id);
	module_info_t *(*mod_list_get)(uint64_t *count);
	uint64_t (*new_thread)(void (*func)(void *), char *name);
	void (*delete_thread)( );
	time_t (*get_time)( );
} __attribute__((packed)) env_t;

#endif // types.h
