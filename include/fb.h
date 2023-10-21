/**
 * fb.h
 * Заголовок с функциями фреймбуффера
 *
 * Данный заголовочный файл содержит определения которые используются для работы
 * с экранным буффером(фреймбуффером)
 *
 */

#ifndef FB_H
#define FB_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

void fb_init( );
void fb_print_buf(size_t x, size_t y, size_t h, size_t w, uint32_t *buf);
void fb_printf(char *str, ...);
void fb_printf_at(size_t x, size_t y, char *str, ...);

#endif // fb.h