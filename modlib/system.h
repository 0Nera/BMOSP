/**
 * system.h
 * Системные вызовы
 *
 * Заголовочный файл содержащий заготовку для работы с системными вызовами
 *
 */

#include <modstd.h>
#include <types.h>

#ifndef SYSTEM_H
#define SYSTEM_H

static void (*fb_printf)(char *str, ...);
static module_info_t *(*get_module)(char *module_id);
static uint64_t offset;

static inline void init_env(env_t *loader_env) {
	offset = loader_env->offset;
	fb_printf = loader_env->fb_printf;
	get_module = loader_env->get_module;
}

#endif // system.h
