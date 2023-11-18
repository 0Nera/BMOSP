ENTRY(init)

SECTIONS
{
	. = 0x1000;
	start = .;

	.text BLOCK(4K) : ALIGN(4K) {
		code = .;
		*(.minit)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K) {
		*(COMMON)
		*(.bss)
	}

	.eh_frame BLOCK(4K) : ALIGN(4K) {
		*(.eh_frame)
	}

	end = .;

	/DISCARD/ : {
		*(.comment)
		*(.note.gnu.build-id)
	}
}