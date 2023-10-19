#include <fb.h>
#include <stdbool.h>
#include <stdint.h>
#include <tool.h>

extern "C" {
void sse_init( ) {
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
}

namespace cpu {
static bool acpi_msrs_support = false;
static bool mmx_support = false;
static bool sse2_support = false;
static bool avx_support = false;
static bool rdrnd_support = false;

void cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx,
           uint32_t *edx) {
	asm volatile("cpuid"
	             : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
	             : "a"(leaf));
}

void msr_get(uint32_t msr, uint32_t *lo, uint32_t *hi) {
	asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void msr_set(uint32_t msr, uint32_t lo, uint32_t hi) {
	asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

uint64_t get_cpu_temperature( ) {
	uint32_t lo, hi;

	// Чтение температуры из MSR
	cpu::msr_get(0x19C, &lo, &hi);

	uint64_t temp = ((uint64_t)hi << 32) | (uint64_t)lo;

	// Преобразование значения температуры
	uint64_t temperature = (temp >> 16) / 256;

	return temperature;
}

void l2_cache( ) {
	unsigned int eax, ebx, ecx, edx;
	unsigned int lsize, assoc, cache;

	cpuid(0x80000005, &eax, &ebx, &ecx, &edx);
	lsize = ecx & 0xFF;
	fb::printf("Кэш L1: %d KB\n", lsize);

	cpuid(0x80000006, &eax, &ebx, &ecx, &edx);
	lsize = ecx & 0xFF;
	assoc = (ecx >> 12) & 0x0F;
	cache = (ecx >> 16) & 0xFF;

	fb::printf("Кэш L2: %d KB\n", lsize);
}

void do_amd( ) {
	uint32_t eax, ebx, ecx, edx;
	uint32_t eggs[4];
	uint32_t cpu_model;
	uint32_t cpu_family;
	char eggs_string[13];

	cpuid(0x8FFFFFFF, &eggs[0], &eggs[1], &eggs[2], &eggs[3]);
	tool::memcpy(eggs_string, eggs, 12);

	cpuid(1, &eax, &ebx, &ecx, &edx);
	cpu_model = (eax >> 4) & 0x0F;
	cpu_family = (eax >> 8) & 0x0F;

	fb::printf("Используется процессор AMD, 0x8FFFFFFF = [%s]\n", eggs_string);
	fb::printf("cpu_model = [%u]\n", cpu_model);
	fb::printf("cpu_family = [%u]\n", cpu_family);
}

void brandname( ) {
	uint32_t eax, ebx, ecx, edx;
	char brand_string[49];
	uint32_t brand[12];
	uint32_t manufacturer[4];
	char manufacturer_string[13];

	cpuid(0, &manufacturer[3], &manufacturer[0], &manufacturer[2],
	      &manufacturer[1]);
	tool::memcpy(manufacturer_string, manufacturer, 12);

	brand_string[48] = 0;
	manufacturer_string[12] = 0;

	fb::printf("[CPUID] manufacturer [%s]\n", manufacturer_string);

	cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
	if (eax >= 0x80000004) {
		cpuid(0x80000002, &brand[0], &brand[1], &brand[2], &brand[3]);
		cpuid(0x80000003, &brand[4], &brand[5], &brand[6], &brand[7]);
		cpuid(0x80000004, &brand[8], &brand[9], &brand[10], &brand[11]);
		tool::memcpy(brand_string, brand, 48);
		fb::printf("[CPUID] 0x80000002:0x80000004 [%s]\n", brand_string);
	}

	if (manufacturer[0] == 0x68747541) { do_amd( ); }
}

void init( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(1, &eax, &ebx, &ecx, &edx);

	if ((edx >> 0) & 1) { fb::printf("FPU(x87) подерживается!\n"); }

	if ((edx >> 22) & 1) {
		acpi_msrs_support = true;
		fb::printf("Встроенный терморегулятор MSRS для ACPI\n");
		fb::printf("Температура: %u\n", get_cpu_temperature( ));
	}

	if ((edx >> 23) & 1) {
		mmx_support = true;
		fb::printf("MMX подерживается!\n");
	}

	if ((edx >> 25) & 1) {
		sse2_support = true;
		fb::printf("SSE2 подерживается!\n");
		// sse_init( );
	}

	cpuid(1, &eax, &ebx, &ecx, &edx);
	if ((edx >> 29) & 1) {
		fb::printf("Термоконтроллер автоматически ограничивает температуру\n");
	}

	if ((ecx >> 28) & 1) {
		avx_support = true;
		fb::printf("AVX подерживается!\n");
	}

	if ((ecx >> 26) & 1) { fb::printf("XSAVE подерживается!\n"); }

	if ((ecx >> 30) & 1) {
		rdrnd_support = true;
		fb::printf("RDRND подерживается!\n");
	}

	cpuid(0x80000001, &eax, &ebx, &ecx, &edx);

	if ((edx >> 11) & 1) { fb::printf("SYSCALL подерживается!\n"); }
	if ((edx >> 29) & 1) { fb::printf("AMD64 подерживается!\n"); }

	brandname( );
	// l2_cache( );
}
} // namespace cpu