#include <system.h>

static char fxsave_region[512] __attribute__((aligned(16)));

static inline void cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx) {
	asm volatile("cpuid"
	             : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
	             : "a"(leaf));
}

static inline void L1_cache_size( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
	if ((edx & 0xFF) == 0) {
		fb_printf("L1 кэш недоступен\n");
		return;
	}
	fb_printf(
	    "L1: Размер строки: %u B, Тип ассоциации: %u, Размер кэша: %u КБ\n",
	    ecx & 0xff, (ecx >> 12) & 0x07, (ecx >> 16) & 0xffff);
}

static inline void L2_cache_size( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
	if ((edx & 0xFF) == 0) {
		fb_printf("L2 кэш недоступен\n");
		return;
	}
	fb_printf(
	    "L2: Размер строки: %u B, Тип ассоциации: %u, Размер кэша: %u КБ\n",
	    ecx & 0xff, (ecx >> 12) & 0x0F, (ecx >> 16) & 0xFFFF);
}

static inline void L3_cache_size( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
	if ((edx & 0xFF) == 0) {
		fb_printf("L3 кэш недоступен\n");
		return;
	}
	fb_printf(
	    "L3: Размер строки: %u B, Тип ассоциации: %u, Размер кэша: %u КБ\n",
	    edx & 0xff, (edx >> 12) & 0x0F, (edx >> 16) & 0xFFFF);
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);

	L1_cache_size( );
	L2_cache_size( );
	L3_cache_size( );

	return (module_info_t){
		.name = (char *)"CPUBENCH",
		.message = (char *)"Дополнительная информация о процессоре",
		.type = 0,
		.data_size = 0,
		.data = (void *)0,
		.data_size = 0,
		.err_code = 0,
		.module_id = 0,
	};
}