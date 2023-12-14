#include <arch.h>
#include <stdint.h>

#define NO_NAME "Не задано название"

#define KERNEL_CODE_SEG 0x08 // Сегмент кода
#define KERNEL_DATA_SEG 0x10 // Сегмент данных

#define IDT_SIZE 256 // Количество обработчиков прерываний
#define IDT_EXCEPTIONS 32

typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint64_t base;
} idt_ptr_t;

typedef struct __attribute__((packed)) {
	uint16_t offset_16;
	uint16_t selector;
	uint8_t ist;
	uint8_t flags;
	uint16_t offset_middle_16;
	uint32_t offset_high_32;
	uint32_t reserved;
} idt_gate_t;

static idt_gate_t idt[256];
int_entry_t isr[256];
extern void *isr_stubs[];
static idt_ptr_t idtr;

const char *exception_names[] = { "Деление на ноль",
	                              "Отладка",
	                              "NMI",
	                              "Точка останова",
	                              "Переполнение",
	                              "Выход за границы",
	                              "Недопустимая операция",
	                              "Устройство недоступно",
	                              "Двойное исключение",
	                              NO_NAME,
	                              "Недопустимый TSS",
	                              "Сегмент не присутствует",
	                              "Ошибка сегмента стека",
	                              "Общая защитная ошибка",
	                              "Ошибка страницы",
	                              NO_NAME,
	                              "x87 исключение",
	                              "Проверка выравнивания",
	                              "Ошибка машины",
	                              "SIMD исключение",
	                              "Виртуализация",
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              NO_NAME,
	                              "Безопасность" };