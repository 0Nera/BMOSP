/**
 * sys.h
 * Заголовок содержащий определения для работы системного API
 *
 * Данный заголовочный файл содержит определения которые используются для
 * объявления структур и системных функций
 *
 */

#ifndef SYS_H
#define SYS_H

#include <stdint.h>

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
} __attribute__((packed)) module_info_t;

typedef struct {
	uint64_t offset;
	void (*fb_printf)(char *str, ...); // Временная функция
	framebuffer_t (*alloc_framebuffer)( );
	void (*free_framebuffer)(framebuffer_t *frame);
	void *(*alloc)(uint64_t size);
	void (*free)(void *ptr);
	void (*exit)(int code);
	int (*get_error)( );
	sys_info_t *(*get_info)( );
	module_info_t *(*get_module)(char *module_id);
	module_info_t *(*mod_list_get)(uint64_t *count);
	uint64_t (*new_thread)(void *func);
	void (*delete_thread)( );
	time_t (*get_time)( );
} __attribute__((packed)) env_t;

env_t *sys_install(env_t *module);

#endif // sys.h