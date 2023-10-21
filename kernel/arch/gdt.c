#include <arch.h>
#include <fb.h>
#include <stdbool.h>
#include <stdint.h>
#include <tool.h>

typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint16_t base_16;
	uint8_t base_middle_16;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_hight_8;
} gdt_entry_t;

typedef struct __attribute__((packed)) {
	uint16_t limit;
	uint64_t base;
} gdt_reg_t;

gdt_entry_t gdt[11];
gdt_reg_t gdtr;

extern void load_gdt(uint64_t gdtr);

void gdt_load( ) {
	gdtr.limit = (sizeof(gdt) - 1);
	gdtr.base = (uint64_t)&gdt;

	load_gdt((uint64_t)&gdtr);
}

void set_gdt_entry(gdt_entry_t *entry, uint16_t limit, uint64_t base,
                   uint8_t access, uint8_t granularity) {
	entry->limit = limit;
	entry->base_16 = base & 0xFFFF;
	entry->base_middle_16 = (base >> 16) & 0xFF;
	entry->base_hight_8 = (base >> 24) & 0xFF;
	entry->access = access;
	entry->granularity = granularity;
}

void gdt_init( ) {
	set_gdt_entry(&gdt[0], 0, 0, 0, 0);
	set_gdt_entry(&gdt[1], 0xFFFF, 0, 0x9A, 0);
	set_gdt_entry(&gdt[2], 0xFFFF, 0, 0x92, 0);
	set_gdt_entry(&gdt[3], 0xFFFF, 0, 0x9A, 0xCF);
	set_gdt_entry(&gdt[4], 0xFFFF, 0, 0x92, 0xCF);
	set_gdt_entry(&gdt[5], 0, 0, 0x9A, 0x20);
	set_gdt_entry(&gdt[6], 0, 0, 0x92, 0);
	set_gdt_entry(&gdt[7], 0, 0, 0xFA, 0x20);
	set_gdt_entry(&gdt[8], 0, 0, 0xF2, 0);
	set_gdt_entry(&gdt[9], 104, 0, 0x89, 0);
	set_gdt_entry(&gdt[10], 0, 0, 0, 0);

	gdt_load( );
	fb_printf("GDT инициализирован\n");
}