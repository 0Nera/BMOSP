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

static void cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
	asm volatile("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "a"(leaf));
}

static void msr_get(uint32_t msr, uint32_t *lo, uint32_t *hi) {
	asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

static void __attribute__((unused)) msr_set(uint32_t msr, uint32_t lo, uint32_t hi) {
	asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

static int64_t get_cpu_temperature_intel( ) {
	uint32_t lo, hi;

	// Чтение температуры из MSR
	msr_get(0x19C, &lo, &hi);

	uint64_t temp = ((uint64_t)hi << 32) | (uint64_t)lo;

	// Преобразование значения температуры
	int64_t temperature = (temp >> 16) / 256;

	if (temperature == 0) { return 0; }
	return ((temperature - 32) * 5 / 9);
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

	LOG("Используется процессор AMD, 0x8FFFFFFF = [%s]\n", eggs_string);
	LOG("cpu_model = [%u]\n", cpu_model);
	LOG("cpu_family = [%u]\n", cpu_family);
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

	LOG("[CPUID] manufacturer [%s]\n", manufacturer_string);

	cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
	if (eax >= 0x80000004) {
		cpuid(0x80000002, &brand[0], &brand[1], &brand[2], &brand[3]);
		cpuid(0x80000003, &brand[4], &brand[5], &brand[6], &brand[7]);
		cpuid(0x80000004, &brand[8], &brand[9], &brand[10], &brand[11]);
		tool_memcpy(brand_string, brand, 48);
		LOG("[CPUID] 0x80000002:0x80000004 [%s]\n", brand_string);
	}

	if (manufacturer[0] == 0x68747541) { do_amd( ); }
}

void cpu_idle( ) {
	if (acpi_msrs_support) { LOG("Температура: %d (в QEMU/KVM всегда 0)\n", get_cpu_temperature_intel( )); }
}

void cpu_init( ) {
	uint32_t eax, ebx, ecx, edx;
	cpuid(1, &eax, &ebx, &ecx, &edx);
	if ((edx >> 22) & 1) {
		acpi_msrs_support = true;
		LOG("Встроенный терморегулятор MSRS для ACPI\n");
		LOG("Температура: %d (в QEMU/KVM всегда 0)\n", get_cpu_temperature_intel( ));
	}

	cpuid(0x80000000, &eax, &ebx, &ecx, &edx);

	LOG("Максимально поддерживаемая функция CPUID = 0x%x (%u)\n", eax, eax);

	cpuid(0x80000001, &eax, &ebx, &ecx, &edx);

	if ((edx >> 5) & 1) { LOG("Регистры MSR подерживаются!\n"); }

	if ((edx >> 9) & 1) {
		LOG("Усовершенствованный программируемый контроллер прерываний "
		    "поддерживается!\n");
	}

	if ((edx >> 10) & 1) { LOG("SYSCALL/SYSRET(для AMD семейства 5 линейки 7) подерживаются!\n"); }
	if ((edx >> 11) & 1) { LOG("SYSCALL/SYSRET подерживаются!\n"); }

	if ((edx >> 29) & 1) { LOG("AMD64 поддерживается!\n"); }

	cpuid(0x80000007, &eax, &ebx, &ecx, &edx);

	if ((edx >> 0) & 1) { LOG("Датчик температуры поддерживается!\n"); }
	if ((edx >> 4) & 1) { LOG("Аппаратный терморегулятор (HTC) поддерживается!\n"); }
	if ((edx >> 5) & 1) { LOG("Программный терморегулятор (STC) поддерживается!\n"); }

	brandname( );
}