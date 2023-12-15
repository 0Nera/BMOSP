/**
 * system.h
 * Системные вызовы
 *
 * Заголовочный файл содержащий заготовку для работы с системными вызовами
 *
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <types.h>

static void *(*alloc)(uint64_t size);
static void (*free)(void *ptr);
static void (*fb_printf)(char *str, ...);
static module_info_t *(*get_module)(char *module_id);
static uint64_t offset;

#include <modstd.h>

static inline void init_env(env_t *loader_env) {
	offset = loader_env->offset;
	fb_printf = loader_env->fb_printf;
	alloc = loader_env->alloc;
	free = loader_env->free;
	get_module = loader_env->get_module;
}

static void *realloc(void *addr, size_t size) {
	if (size == 0) {
		free(addr);
		return NULL;
	}

	if (addr == NULL) { return alloc(size); }

	void *new_addr = alloc(size);

	if (new_addr == NULL) { return NULL; }

	memcpy(new_addr, addr, size);
	free(addr);

	return new_addr;
}

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
	asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
	uint16_t ret;
	asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline uint32_t inl(uint16_t port) {
	uint32_t data;
	asm volatile("inl %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}

static inline void outl(uint16_t port, uint32_t data) {
	asm volatile("outl %0, %1" : : "a"(data), "Nd"(port));
}

#endif // system.h
