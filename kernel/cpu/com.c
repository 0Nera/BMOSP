/**
 * com.c
 * последовательный порт
 *
 * Функционал получения и записи информации в последовательный порт
 *
 */

#include <arch.h>

static inline int com_is_transmit_empty(uint16_t com) {
	return inb(com + 5) & 0x20;
}

void com_write_byte(char byte) {
	while (!com_is_transmit_empty(0x3F8)) {}

	outb(0x3F8, byte);
}

void com_write_bytes(char *c, uint64_t n) {
	for (uint64_t i = 0; i < n; i++) { com_write_byte(c[i]); }
}