#include <system.h>

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static inline void usleep(uint64_t ticks) {
	for (uint64_t i = 0; i < ticks * 100; i++) { asm volatile("pause"); }
}

static inline void play_sound(unsigned int frequency) {}

int init(env_t *env) {
	init_env(env);
	return 0;
	// Массив с нотами
	unsigned int tetris_notes[] = { 0 };

	// Расчет количества нот в мелодии
	int num_notes = sizeof(tetris_notes) / sizeof(tetris_notes[0]);

	// Начальное значение для подсчета времени
	int note_duration = 1000000; // 1 секунда

	// Зацикленное воспроизведение мелодии в течение минуты
	for (int i = 0; i < num_notes; i++) {
		// Воспроизведение ноты с заданным временем
		play_sound(tetris_notes[i]);
		usleep(note_duration);

		// Уменьшение времени для следующей ноты
		note_duration -= note_duration / 24; // Уменьшение на 1/24 каждый раз
	}

	return 0;
}