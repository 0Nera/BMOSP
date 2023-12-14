#include <system.h>

static void handler( ) {
	for (;;) {
		if (!(inb(0x64) & 1)) { continue; }
		uint64_t byte = inb(0x60);
		switch (byte) {
			case 0x1: // Клавиша "ESCAPE"
				fb_printf("Выход для Bochs\n");
				outw(0xB004, 0x2000);

				fb_printf("Выход для Qemu\n");
				outw(0x604, 0x2000);

				fb_printf("Выход для Virtualbox\n");
				outw(0x4004, 0x3400);

				fb_printf("Выход для облачного гипервизора\n");
				outw(0x600, 0x34);
				break;
			case 0x4F: // Клавиша "END"
				fb_printf("Вызов прерывания переключения задач!\n");
				asm volatile("int $32");
				break;
			default: break;
		}

		uint8_t status = inb(0x61);

		status |= 1;

		outb(0x61, status);
	}
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);

	return (module_info_t){ .name = (char *)"[KEYBOARD]",
		                    .message = (char *)"PS/2 драйвер",
		                    .type = 0,
		                    .data_size = 0,
		                    .data = (void *)0,
		                    .err_code = 0,
		                    .module_id = 0,
		                    .irq = 33,
		                    .irq_handler = handler };
}