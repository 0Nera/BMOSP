/**
 * io_buf.h
 * Заголовок с определениями функций буфера ввода-вывода
 *
 * Данный заголовочный файл содержит определения которые используются для
 * использования буфера ввода-вывода
 *
 */

#ifndef IO_BUF_H
#define IO_BUF_H

#include <stdint.h>

typedef struct {
	uint8_t *buffer;
	uint64_t position;
	uint64_t max_size;
} io_buf_t;

io_buf_t *io_buf_create(uint64_t max_size);
void io_buf_destroy(io_buf_t *buffer);
void io_buf_write(io_buf_t *buffer, uint8_t *data, uint64_t length);
uint8_t *io_buf_read(io_buf_t *buffer);

#endif // io_buf.h