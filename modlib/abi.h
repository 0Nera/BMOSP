/**
 * abi.h
 * Единый ABI между ядром и модулями
 *
 * Заголовочный файл с каноническими определениями структур и констант,
 * которые передаются между ядром и загружаемыми модулями
 *
 */

#ifndef BMOSP_ABI_H
#define BMOSP_ABI_H

#include <stdint.h>

#define BMOSP_ABI_MAGIC 0x424D4F53ULL
#define BMOSP_ABI_VERSION 1U
#define BMOSP_ABI_FLAGS 0U

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
	uint8_t irq;       // Номер прерывания
	void *irq_handler; // Адрес обработчика прерываний
	void *(*get_func)(uint64_t id);
	void (*after_init)( );
	void *env; // env_t
} __attribute__((packed)) module_info_t;

typedef struct env_t_s {
	uint64_t abi_magic;
	uint32_t abi_version;
	uint32_t abi_flags;
	uint64_t abi_size;
	uint64_t module_info_size;
	uint64_t offset;
	uint64_t id;
	void (*log_printf)(char *str, ...);
	framebuffer_t (*alloc_framebuffer)( );
	void (*free_framebuffer)(framebuffer_t *frame);
	void *(*alloc)(uint64_t size);
	void (*free)(void *ptr);
	void (*exit)(int code);
	int (*get_error)( );
	sys_info_t *(*get_info)( );
	module_info_t *(*get_module)(char *module_id);
	module_info_t *(*mod_list_get)(uint64_t *count);
	uint64_t (*new_thread)(void (*func)(void *), char *name, void *arg);
	void (*delete_thread)( );
	void (*task_switch)( );
	time_t (*get_time)( );
	void (*set_int)(uint8_t vector, int_entry_t handler);
	void (*mod_update_info)(struct env_t_s *ret);
	module_info_t *ret;
} __attribute__((packed)) env_t;

typedef void (*module_entry_t)(env_t *env);

#endif // BMOSP_ABI_H
