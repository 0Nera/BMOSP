#include <limine.h>



// Точка входа
extern "C" void _start() {
    asm volatile("cli");

    for (;;) {
        asm volatile("hlt");
    }
}