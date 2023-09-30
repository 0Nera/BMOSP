#include <limine.h>
#include <arch.h>
#include <cpu.h>
#include <fb.h>
#include <tool.h>



// Точка входа
extern "C" void _start() {
    asm volatile("cli");

    arch::init();
    cpu::init();
    fb::init();

    for (;;) {
        asm volatile("hlt");
    }
}