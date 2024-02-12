.global load_gdt
load_gdt:
		cli
		lgdt (%rdi)		# Загружаем GDT
		movw $0x30, %ax # Обновляем GDT
		mov %ax, %ds
		mov %ax, %es
		mov %ax, %fs
		mov %ax, %gs
		mov %ax, %ss
		pop %rdi
		mov $0x28, %rax
		push %rax
		push %rdi
		retfq
