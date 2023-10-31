/**
 * mem.h
 * Заголовочный файл с функциями менеджера памяти
 *
 * Данный заголовочный файл содержит определения, которые используются для
 * работы с памятью
 *
 */

#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

#define BLOCK_SIZE 4096
#define HHDM_OFFSET (hhdm_request.response->offset)

void mem_dump_memory( );
void mem_init( );
void *mem_alloc(size_t size);
void mem_add_block(void *addr, size_t size);
void mem_free(void *addr);
void *mem_realloc(void *addr, size_t size);
void *mem_frame_alloc(uint64_t wanted_frames);
void mem_frame_free(void *ptr, uint64_t frames);
void *mem_frame_calloc(uint64_t frames);
void mem_merge_all_blocks( );

#endif // mem.h