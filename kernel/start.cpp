#include <arch.h>
#include <cpu.h>
#include <fb.h>
#include <limine.h>
#include <mem.h>
#include <mod.h>
#include <tool.h>

// Точка входа
extern "C" void _start( ) {
	asm volatile("cli");

	fb::init( );
	fb::printf("Привет, мир!!\n");
	cpu::init( );
	mem::init( );

	for (;;) { asm volatile("hlt"); }
}