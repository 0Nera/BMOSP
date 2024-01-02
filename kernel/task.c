/**
 * task.c
 * Управление потоками
 *
 * Данный файл содержит функции для создания и удаления потоков.
 *
 */

#include <arch.h>
#include <fb.h>
#include <mem.h>

static volatile uint64_t pid = 0;
static task_t *kernel_task = NULL;
task_t *current_task = NULL;

void task_switch(struct frame *state) {
	// Смена потоков

	task_t *prev_task = current_task;

	if (current_task == NULL) { return; }

	LOG("Смена потоков %u -> ", current_task->id);
	current_task = current_task->next;

	if (current_task == NULL) {
		// LOG("current_task == NULL\n");

		// LOG("current_task = kernel_task\n");
		current_task = kernel_task;
	}

	fb_printf("%u\n", current_task->id);

	// Сохранение предыдущего состояния
	prev_task->state->rip = state->rip;
	prev_task->state->rsp = state->rsp;
	prev_task->state->rbp = state->rbp;

	// Восстановление следующего состояния
	// LOG("Смена rsp\n");
	state->rsp = current_task->state->rsp;
	// LOG("Смена rbp\n");
	state->rbp = current_task->state->rbp;
	// LOG("Смена rip\n");
	state->rip = current_task->state->rip;
	// LOG("Конец!\n");
}

task_t *task_new_thread(void (*func)(void *), void *arg) {
	LOG("Выделение потока\n");

	task_t *new_task = mem_alloc(sizeof(task_t));

	LOG("new_task = 0x%x\n", new_task);
	new_task->id = ++pid;
	// mem_dump_memory( );
	LOG("Выделение состояния\n");
	new_task->state = mem_alloc(sizeof(struct frame));
	new_task->next = NULL;

	// Выделяем память под стек
	LOG("Выделение стека\n");
	void *stack = mem_alloc(STACK_SIZE);

	// Устанавливаем значения регистров для нового потока
	LOG("Установка регистров\n");
	new_task->state->rbp = (uint64_t)stack + STACK_SIZE; // Указываем на вершину стека
	new_task->state->rbx = 0;
	new_task->state->r15 = 0;
	new_task->state->r14 = 0;
	new_task->state->r13 = 0;
	new_task->state->r12 = 0;
	new_task->state->r11 = 0;
	new_task->state->r10 = 0;
	new_task->state->r9 = 0;
	new_task->state->r8 = 0;
	new_task->state->rax = 0;
	new_task->state->rcx = 0;
	new_task->state->rdx = 0;
	new_task->state->rsi = 0;
	new_task->state->rdi = (uint64_t)arg; // Передаем аргументы в регистр rdi
	new_task->state->int_number = 0;
	new_task->state->err = 0;
	new_task->state->rip = (uint64_t)func; // Устанавливаем адрес функции
	new_task->state->cs = 0;
	new_task->state->rflags = 0;
	new_task->state->rsp = (uint64_t)stack + STACK_SIZE; // Указываем на вершину стека
	new_task->state->ss = 0;

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

	return new_task;
}

void task_init( ) {
	idt_set_int(32, task_switch);
	LOG("Потоки инициализированы\n");
}
