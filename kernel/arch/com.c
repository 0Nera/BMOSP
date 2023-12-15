#include <arch.h>

static inline int serial_is_transmit_empty(uint16_t com) {
	return inb(com + 5) & 0x20;
}

void serial_write_byte(uint8_t byte) {
	while (!serial_is_transmit_empty(0x3F8)) {}

	outb(0x3F8, byte);
}

void serial_write_bytes(char *c, uint64_t n) {
	for (uint64_t i = 0; i < n; i++) { serial_write_byte(c[i]); }
}