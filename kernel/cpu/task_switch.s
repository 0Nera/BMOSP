.global task_switch_asm
task_switch_asm:
	pushfq
	movq %rax,   (%rdi)
	movq %rbx,  8(%rdi)
	movq %rcx, 16(%rdi)
	movq %rdx, 24(%rdi)
	movq %rsi, 32(%rdi)
	movq %rdi, 40(%rdi)
	movq %rsp, 48(%rdi)
	movq %rbp, 56(%rdi)
	movq %cr3, %rax
	movq %rax, 64(%rdi)

	movq 64(%rsi), %rax
	movq %rax, %cr3
	movq 56(%rsi), %rbp
	movq 48(%rsi), %rsp
	movq 40(%rsi), %rdi
	movq 24(%rsi), %rdx
	movq 16(%rsi), %rcx
	movq  8(%rsi), %rbx
	movq   (%rsi), %rax
	movq 32(%rsi), %rsi
	popfq

	retq