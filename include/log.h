/**
 * log.h
 * Заголовок с функциями отладки
 *
 * Данный заголовочный файл содержит определения которые используются для работы
 * с системным отладчиком
 *
 */

#ifndef LOG_H
#define LOG_H

#include <6x8_slim_font.h>
#include <arch.h>
#include <fb.h>
#include <lock.h>
#include <mem.h>
#include <stdint.h>
#include <tool.h>

void log_init( );
void log_init_mem( );
void log_printf(char *str, ...);

#ifndef NO_DEBUG
#define LOG(...)                                                                                                       \
	log_printf("[%6u]%12s() (%18s:%3u) ", GET_TICK_BIG, __func__, __FILE__, __LINE__);                                 \
	log_printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#endif // log.h
