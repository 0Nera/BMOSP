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
	char *name;
	char *message;
	int err_code;
	uint64_t func_count;
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

typedef struct {
	uint64_t offset;
	module_info_t *info;
	void (*fb_printf)(char *str, ...);
} env_t;

#endif // sys.h