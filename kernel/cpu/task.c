/**
 * task.c
 * Управление потоками
 *
 * Данный файл содержит функции для создания и удаления потоков.
 *
 */

#include <arch.h>
#include <log.h>
#include <mem.h>

static volatile uint64_t next_thread_id = 0;
static task_t *last_task = NULL;
static task_t *kernel_task = NULL;
task_t *current_task = NULL;

void task_switch(struct frame *state) {
	// Смена потоков
	asm volatile("cli");
	asm volatile("pushf");

	task_t *prev_task = current_task;

	if (current_task->next == NULL) { current_task = kernel_task; }

	LOG("Смена потоков %u -> ", current_task->id);

	asm volatile("mov %%rsp, %0" : "=a"(current_task->rsp));

	current_task = current_task->next;

	if (current_task == NULL) {
		// LOG("current_task == NULL\n");

		// LOG("current_task = kernel_task\n");
		current_task = kernel_task;
	}

	LOG("%u\n", current_task->id);

	asm volatile("mov %0, %%rsp" ::"a"(current_task->rsp));
	asm volatile("popf");
}

task_t *task_new_thread(void (*func)(void *), void *arg) {
	uint64_t eflags;
	void *stack = NULL;

	asm volatile("cli");

	LOG("Выделение потока\n");

	asm volatile("pushf");
	asm volatile("pop %0" : "=r"(eflags));

	task_t *new_task = mem_alloc(sizeof(task_t));

	tool_memset(new_task, 0, sizeof(task_t));

	new_task->id = next_thread_id++;
	new_task->stack_size = STACK_SIZE;
	new_task->entry_point = func;

	new_task->priority = 1;

	stack = mem_alloc(STACK_SIZE);

	new_task->stack = stack;
	new_task->rsp = (uintptr_t)stack + STACK_SIZE - 16;

	uintptr_t *rsp = (uintptr_t *)stack;
	*(--rsp) = (uintptr_t)func;   // Добавляем entry_point на стек
	*(--rsp) = eflags | (1 << 9); // Сохраняем флаги на стеке

	// Добавляем new_task в цепочку
	if (last_task != NULL) { last_task->next = new_task; }
	new_task->last = last_task;
	new_task->next = NULL;
	last_task = new_task;

	if (kernel_task == NULL) {
		LOG("Ядро ID: %u\n", new_task->id);
		kernel_task = new_task;
		current_task = new_task;
	}

	if (current_task != new_task) {
		LOG("Прошлый ID: %u\n", current_task->id);
		current_task->next = new_task;
	}

	LOG("Создан новый поток с ID: %u\n", new_task->id);
	asm volatile("sti"); // Включаем прерывания

	return new_task;
}

void notask_switch( ) {
	asm volatile("nop");
}

void task_init( ) {
	LOG("Потоки не инициализированы\n");
	idt_set_int(32, notask_switch);
	return;
	uint64_t rsp;
	uint64_t cr3;

	asm volatile("mov %%rsp, %0" : "=r"(rsp));

	asm volatile("mov %%cr3, %0" : "=r"(cr3));
	asm volatile("cli");

	kernel_task = mem_alloc(sizeof(task_t));
	tool_memset(kernel_task, 0, sizeof(task_t));
	kernel_task->id = next_thread_id++;
	kernel_task->stack_size = STACK_SIZE;
	kernel_task->rsp = rsp;

	current_task = kernel_task;
	last_task = kernel_task;

	LOG("Потоки инициализированы\n");
}
