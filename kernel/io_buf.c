/**
 * io_buf.c
 * Функции буфера ввода-вывода
 *
 * Функционал буфера ввода-вывода
 *
 */

#include <io_buf.h>
#include <log.h>
#include <mem.h>
#include <stdint.h>
#include <tool.h>

io_buf_t *io_buf_create(uint64_t max_size) {
	io_buf_t *buffer = mem_alloc(sizeof(io_buf_t));
	buffer->buffer = mem_alloc(max_size * sizeof(uint8_t));
	buffer->position = 0;
	buffer->max_size = max_size;
	return buffer;
}

void io_buf_destroy(io_buf_t *buffer) {
	mem_free(buffer->buffer);
	mem_free(buffer);
}

void io_buf_write(io_buf_t *buffer, uint8_t *data, uint64_t length) {
	if (buffer->position + length > buffer->max_size) {
		LOG("Переполнение буффера\n");
		return;
	}

	tool_memcpy(buffer->buffer + buffer->position, data, length);
	buffer->position += length;
}

uint8_t *io_buf_read(io_buf_t *buffer) {
	uint8_t *data = mem_alloc((buffer->position + 1) * sizeof(uint8_t));

	tool_memcpy(data, buffer->buffer, buffer->position);
	data[buffer->position] = '\0';
	buffer->position = 0;
	tool_memset(buffer->buffer, 0, buffer->max_size);

	return data;
}
