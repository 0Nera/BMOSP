/**
 * cpu.c
 * Информация о процессоре
 *
 * Функционал получения дополнительной информации о процессоре и доступных
 * процессорных инструкциях
 *
 */

#include <fb.h>
#include <stdbool.h>
#include <stdint.h>
#include <tool.h>

static bool acpi_msrs_support = false;
static bool mmx_support = false;
static bool sse2_support = false;
static bool avx_support = false;
static bool rdrnd_support = false;

static void sse_init( ) {
	uint64_t _cr0 = 0;
	asm volatile("mov %0, %%cr0" : "=r"(_cr0) : : "memory");
	_cr0 &= ~(1 << 2);
	_cr0 |= (1 << 1);
	asm volatile("mov %%cr0, %0" : : "r"(_cr0) : "memory");

	uint64_t _cr4 = 0;
	asm volatile("mov %0, %%cr4" : "=r"(_cr4) : : "memory");
	_cr4 |= (3 << 9);
	asm volatile("mov %%cr4, %0" : : "r"(_cr4) : "memory");
}

static void cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
	asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "a"(leaf));
}

static void msr_get(uint32_t msr, uint32_t *lo, uint32_t *hi) {
	asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

static void msr_set(uint32_t msr, uint32_t lo, uint32_t hi) {
	asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

static uint64_t get_cpu_temperature( ) {
	uint32_t lo, hi;

	// Чтение температуры из MSR
	msr_get(0x19C, &lo, &hi);

	uint64_t temp = ((uint64_t)hi << 32) | (uint64_t)lo;

	// Преобразование значения температуры
	uint64_t temperature = (temp >> 16) / 256;

	return temperature;
}

static void l2_cache( ) {
	unsigned int eax, ebx, ecx, edx;
	unsigned int lsize, assoc, cache;

	cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
	lsize = ecx & 0xFF;
	assoc = (ecx >> 12) & 0x07;
	cache = (ecx >> 16) & 0xFFFF;

	fb_printf("Размер строки: %u B, Тип ассоциации: %u, Размер кэша: %u КБ\n", lsize, assoc, cache);
}

static void do_amd( ) {
	uint32_t eax, ebx, ecx, edx;
	uint32_t eggs[4];
	uint32_t cpu_model;
	uint32_t cpu_family;
	char eggs_string[13];

	cpuid(0x8FFFFFFF, &eggs[0], &eggs[1], &eggs[2], &eggs[3]);
	tool_memcpy(eggs_string, eggs, 12);

	cpuid(1, &eax, &ebx, &ecx, &edx);
	cpu_model = (eax >> 4) & 0x0F;
	cpu_family = (eax >> 8) & 0x0F;

	fb_printf("Используется процессор AMD, 0x8FFFFFFF = [%s]\n", eggs_string);
	fb_printf("cpu_model = [%u]\n", cpu_model);
	fb_printf("cpu_family = [%u]\n", cpu_family);
}

static void brandname( ) {
	uint32_t eax, ebx, ecx, edx;
	char brand_string[49];
	uint32_t brand[12];
	uint32_t manufacturer[4];
	char manufacturer_string[13];

	cpuid(0, &manufacturer[3], &manufacturer[0], &manufacturer[2], &manufacturer[1]);
	tool_memcpy(manufacturer_string, manufacturer, 12);

	brand_string[48] = 0;
	manufacturer_string[12] = 0;

	fb_printf("[CPUID] manufacturer [%s]\n", manufacturer_string);

	cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
	if (eax >= 0x80000004) {
		cpuid(0x80000002, &brand[0], &brand[1], &brand[2], &brand[3]);
		cpuid(0x80000003, &brand[4], &brand[5], &brand[6], &brand[7]);
		cpuid(0x80000004, &brand[8], &brand[9], &brand[10], &brand[11]);
		tool_memcpy(brand_string, brand, 48);
		fb_printf("[CPUID] 0x80000002:0x80000004 [%s]\n", brand_string);
	}

	if (manufacturer[0] == 0x68747541) { do_amd( ); }
}

void cpu_init( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(1, &eax, &ebx, &ecx, &edx);

	if ((edx >> 0) & 1) { fb_printf("FPU(x87) поддерживается!\n"); }

	if ((edx >> 22) & 1) {
		acpi_msrs_support = true;
		fb_printf("Встроенный терморегулятор MSRS для ACPI\n");
		fb_printf("Температура: %u (в QEMU/KVM всегда 0)\n", get_cpu_temperature( ));
	}

	if ((edx >> 23) & 1) {
		mmx_support = true;
		fb_printf("MMX поддерживается!\n");
	}

	if ((edx >> 25) & 1) {
		sse2_support = true;
		fb_printf("SSE2 поддерживается!\n");
		// sse_init( );
	}

	cpuid(1, &eax, &ebx, &ecx, &edx);
	if ((edx >> 29) & 1) { fb_printf("Термоконтроллер автоматически ограничивает температуру\n"); }

	if ((ecx >> 28) & 1) {
		avx_support = true;
		fb_printf("AVX поддерживается!\n");
	}

	if ((ecx >> 26) & 1) { fb_printf("XSAVE поддерживается!\n"); }

	if ((ecx >> 30) & 1) {
		rdrnd_support = true;
		fb_printf("RDRND поддерживается!\n");
	}

	cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
	fb_printf("Максимально поддерживаемая функция CPUID = 0x%x (%u)\n", eax, eax);

	cpuid(0x80000001, &eax, &ebx, &ecx, &edx);

	if ((edx >> 5) & 1) { fb_printf("Регистры MSR подерживаются!\n"); }

	// if ((edx >> 6) & 1) { fb_printf("Расширение физического адреса поддерживается!\n"); }

	// if ((edx >> 7) & 1) { fb_printf("Исключение проверки компьютера (MCE) поддерживается!\n"); }

	if ((edx >> 9) & 1) {
		fb_printf("Усовершенствованный программируемый контроллер прерываний "
		          "поддерживается!\n");
	}

	if ((edx >> 10) & 1) {
		fb_printf("SYSCALL/SYSRET(для AMD семейства 5 линейки 7) подерживаются!\n");
	}
	if ((edx >> 11) & 1) { fb_printf("SYSCALL/SYSRET подерживаются!\n"); }

	// if ((edx >> 26) & 1) { fb_printf("Гигабайтные страницы подерживаются!\n"); }

	if ((edx >> 29) & 1) { fb_printf("AMD64 поддерживается!\n"); }
	// if ((edx >> 30) & 1) { fb_printf("\"3DNow!\" поддерживается!\n"); }
	// if ((edx >> 31) & 1) { fb_printf("\"Extended 3DNow!\" поддерживается!\n"); }
	if ((ecx >> 6) & 1) { fb_printf("SSE4a поддерживается!\n"); }
	if ((ecx >> 7) & 1) { fb_printf("Смещенный режим SSE поддерживается!\n"); }

	cpuid(0x80000007, &eax, &ebx, &ecx, &edx);
	// if ((ebx >> 0) & 1) { fb_printf("Восстановление после переполнения MCA поддерживается!\n"); }
	// if ((ebx >> 1) & 1) { fb_printf("Возможность локализации и восстановления неисправимых
	// программных ошибок поддерживается!\n"); }
	if ((edx >> 0) & 1) { fb_printf("Датчик температуры поддерживается!\n"); }
	if ((edx >> 3) & 1) { fb_printf("Терморегулятор поддерживается!\n"); }
	if ((edx >> 4) & 1) { fb_printf("Аппаратный терморегулятор (HTC) поддерживается!\n"); }
	if ((edx >> 5) & 1) { fb_printf("Программный терморегулятор (STC) поддерживается!\n"); }
	if ((edx >> 6) & 1) { fb_printf("Управление множителем 100 МГц поддерживается!\n"); }

	// fb_printf("0x80000007[ECX] = 0x%x (%u)\n", ecx, ecx);

	cpuid(0xC0000000, &eax, &ebx, &ecx, &edx);
	if (eax > 0xC0000000) { fb_printf("0xC0000000 [EAX] = 0x%x (%u)\n", eax, eax); }

	brandname( );
	l2_cache( );
}