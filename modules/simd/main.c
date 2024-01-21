#include <system.h>

static char fxsave_region[512] __attribute__((aligned(16)));

static inline void cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
	asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "a"(leaf));
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);

	uint32_t eax, ebx, ecx, edx;
	cpuid(1, &eax, &ebx, &ecx, &edx);

	if ((edx >> 0) & 1) {
		asm volatile("finit");
		fb_printf("FPU(x87) поддерживается!\n");
	}

	if ((edx >> 23) & 1) { fb_printf("MMX поддерживается!\n"); }

	if ((edx >> 25) & 1) {
		fb_printf("SSE2 поддерживается!\n");
		fb_printf("Адрес региона fxsave 0x%x\n", &fxsave_region);
		asm volatile(" fxsave %0 " ::"m"(fxsave_region));
		uint32_t sse_version = (ecx >> 25) & 0x7;
		fb_printf("SSE%u включен\n", sse_version);
	}

	if ((ecx >> 28) & 1) { fb_printf("AVX поддерживается!\n"); }

	return (module_info_t){ .name = (char *)"SIMD",
		                    .message = (char *)"SIMD инструкции",
		                    .type = 0,
		                    .data_size = 0,
		                    .data = (void *)0,
		                    .err_code = 0,
		                    .module_id = 0,
		                    .irq = 0,
		                    .irq_handler = 0,
		                    .get_func = 0,
		                    .after_init = 0 };
}