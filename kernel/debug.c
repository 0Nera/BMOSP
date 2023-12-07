/**
 * debug.c
 * Функции отладочных сообщений
 *
 * Функционал управления выводом отладочных сообщений
 *
 */

#include <fb.h>
#include <mem.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <tool.h>

typedef struct msg {
	uint64_t level;
	char *message;
	struct msg *next;
} msg_t;

msg_t *log = NULL;
msg_t *last_message = NULL;

uint32_t level_colors[] = {
	0x0000FF00, // Зеленый
	0x00FFFF00, // Желтый
	0x00FF0000, // Красный
	0x0000FFFF, // Голубой
	0x000000FF, // Синий
	0x00FF00FF  // Пурпурный
};

// Функция для добавления сообщения в лог
void add_message(uint64_t level, char *message) {
	msg_t *new_msg = (msg_t *)mem_alloc(sizeof(msg_t));

	new_msg->level = level;
	new_msg->message = mem_alloc(tool_strlen(message) + 1);
	tool_strcpy(new_msg->message, message);

	new_msg->next = NULL;

	if (log == NULL) {
		log = new_msg;
	} else {
		msg_t *current = log;
		while (current->next != NULL) { current = current->next; }
		current->next = new_msg;
	}
	last_message = new_msg;
}

// Функция для добавления символа в сообщение
void add_char_to_message(char c, msg_t *msg) {
	uint64_t length = tool_strlen(msg->message);
	msg->message = mem_realloc(msg->message, length + 1);
	msg->message[length] = c;
	msg->message[length + 1] = '\0';
}

void debug_putc(char c) {
	add_char_to_message(c, last_message);
}

void debug_printf(uint8_t level, char *info, char *format, ...) {
	va_list args;
	va_start(args, format);
	add_message(level, info);
	tool_format(debug_putc, format, args);
	va_end(args);
}

// Функция для вывода всего лога
void print_log( ) {
	msg_t *current = log;
	while (current != NULL) {
		fb_set_text_color(level_colors[current->level]);
		fb_printf("Message: %s\n", current->message);
		fb_set_text_color(0x00D000);
		current = current->next;
	}
}