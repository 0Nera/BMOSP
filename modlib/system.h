/**
 * system.h
 * Системные вызовы
 *
 * Заголовочный файл содержащий заготовку для работы с системными вызовами
 *
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <modstd.h>
#include <types.h>

extern void *(*alloc)(uint64_t size);
extern void (*free)(void *ptr);
extern void (*log_printf)(char *str, ...);
extern module_info_t *(*get_module)(char *module_id);
extern module_info_t *(*mod_list_get)(uint64_t *count);
extern framebuffer_t (*alloc_framebuffer)( );
extern void (*free_framebuffer)(framebuffer_t *frame);
extern void (*exit)(int code);
extern int (*get_error)( );
extern sys_info_t *(*get_info)( );
extern uint64_t (*new_thread)(void (*func)(void *), char *name, void *arg);
extern void (*delete_thread)( );
extern void (*task_switch)( );
extern time_t (*get_time)( );
extern void (*mod_update_info)(env_t *ret);
extern void (*set_int)(uint8_t vector, void (*func)(void *));
extern uint64_t offset;

void init_env(env_t *loader_env);
void *realloc(void *addr, size_t size);

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
