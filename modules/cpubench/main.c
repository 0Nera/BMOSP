#include <system.h>

static char fxsave_region[512] __attribute__((aligned(16)));

static inline void cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
	asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "a"(leaf));
}

static inline void L1_cache_size( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
	if ((edx & 0xFF) == 0) {
		log_printf("L1 кэш недоступен\n");
		return;
	}
	log_printf("L1: Размер строки: %u B, Тип ассоциации: %u, Размер кэша: %u КБ\n", ecx & 0xFF, (ecx >> 12) & 0x07,
	           (ecx >> 16) & 0xFFff);
}

static inline void L2_cache_size( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
	if ((edx & 0xFF) == 0) {
		log_printf("L2 кэш недоступен\n");
		return;
	}
	log_printf("L2: Размер строки: %u B, Тип ассоциации: %u, Размер кэша: %u КБ\n", ecx & 0xFF, (ecx >> 12) & 0x0F,
	           (ecx >> 16) & 0xFFFF);
}

static inline void L3_cache_size( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
	if ((edx & 0xFF) == 0) {
		log_printf("L3 кэш недоступен\n");
		return;
	}
	log_printf("L3: Размер строки: %u B, Тип ассоциации: %u, Размер кэша: %u КБ\n", edx & 0xFF, (edx >> 12) & 0x0F,
	           (edx >> 16) & 0xFFFF);
}

static void cpu_info( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(0x8000001E, &eax, &ebx, &ecx, &edx);

	uint32_t apic_id = eax;
	uint8_t compute_unit_id = ebx & 0xFF;
	uint8_t cores_per_compute_unit = ((ebx >> 8) & 3) + 1;
	uint8_t node_id = ecx & 0xFF;
	uint8_t nodes_per_processor = ((ecx >> 8) & 3) + 1;

	log_printf("Топология процессора:\n");
	log_printf("  Идентификатор APIC: %u\n", apic_id);
	log_printf("  Идентификатор вычислительной единицы: %u\n", compute_unit_id);
	log_printf("  Ядра на вычислительную единицу: %u\n", cores_per_compute_unit);
	log_printf("  Идентификатор узла: %u\n", node_id);
	log_printf("  Узлы на процессор: %u\n", nodes_per_processor);
}

void __attribute__((section(".minit"))) init(env_t *env) {
	uint32_t eax, ebx, ecx, edx;

	init_env(env);

	cpuid(1, &eax, &ebx, &ecx, &edx);

	if (ecx & (1 << 31)) {
		log_printf("Запуск на эмуляторе\n");
	} else {
		log_printf("Запуск на физическом процессоре\n");
	}

	L1_cache_size( );
	L2_cache_size( );
	L3_cache_size( );

	cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
	if (eax >= 0x8000001E) { cpu_info( ); }

	env->ret = &((module_info_t){ .name = (char *)"CPUBENCH",
	                              .message = (char *)"Дополнительная информация о процессоре",
	                              .type = 0,
	                              .data_size = 0,
	                              .data = (void *)0,
	                              .err_code = 0,
	                              .module_id = 0,
	                              .irq = 0,
	                              .irq_handler = 0,
	                              .get_func = 0,
	                              .after_init = 0 });
	delete_thread( );
}