/**
 * rtc.c
 * Часы реального времени
 *
 * Функции даты и времени с точностью до секунды
 *
 */

#include <lock.h>
#include <log.h>
#include <stdint.h>
#include <sys.h>
#include <tool.h>

#define CMOS_ADDR_PORT 0x70
#define CMOS_DATA_PORT 0x71

static int century_register = 0;
static uint8_t century = 20;

static inline uint8_t read_cmos(uint8_t addr) {
	outb(CMOS_ADDR_PORT, addr);
	return inb(CMOS_DATA_PORT);
}

static inline int get_update_in_progress_flag( ) {
	outb(CMOS_ADDR_PORT, 0x0A);
	return (inb(CMOS_DATA_PORT) & 0x80);
}

time_t rtc_get_time( ) {
	uint8_t last_second, last_minute, last_hour, last_day, last_month, last_year;
	time_t rtc_time;

	while (get_update_in_progress_flag( ))
		;

	rtc_time.second = read_cmos(0x00);
	rtc_time.minutes = read_cmos(0x02);
	rtc_time.hours = read_cmos(0x04);
	rtc_time.day = read_cmos(0x07);
	rtc_time.month = read_cmos(0x08);
	rtc_time.year = read_cmos(0x09);

	if (century_register != 0) { rtc_time.year += century * 100; }

	do {
		last_second = rtc_time.second;
		last_minute = rtc_time.minutes;
		last_hour = rtc_time.hours;
		last_day = rtc_time.day;
		last_month = rtc_time.month;
		last_year = rtc_time.year;

		while (get_update_in_progress_flag( ))
			;

		rtc_time.second = read_cmos(0x00);
		rtc_time.minutes = read_cmos(0x02);
		rtc_time.hours = read_cmos(0x04);
		rtc_time.day = read_cmos(0x07);
		rtc_time.month = read_cmos(0x08);
		rtc_time.year = read_cmos(0x09);

		if (century_register != 0) { rtc_time.year += century * 100; }
	} while ((last_second != rtc_time.second) || (last_minute != rtc_time.minutes) || (last_hour != rtc_time.hours) ||
	         (last_day != rtc_time.day) || (last_month != rtc_time.month) || (last_year != rtc_time.year));

	uint8_t reg_b = read_cmos(0x0B);

	// Преобразуем BCD значения в бинарные, если необходимо
	if (!(reg_b & 0x04)) {
		rtc_time.second = (rtc_time.second & 0x0F) + ((rtc_time.second / 16) * 10);
		rtc_time.minutes = (rtc_time.minutes & 0x0F) + ((rtc_time.minutes / 16) * 10);
		rtc_time.hours = ((rtc_time.hours & 0x0F) + (((rtc_time.hours & 0x70) / 16) * 10)) | (rtc_time.hours & 0x80);
		rtc_time.day = (rtc_time.day & 0x0F) + ((rtc_time.day / 16) * 10);
		rtc_time.month = (rtc_time.month & 0x0F) + ((rtc_time.month / 16) * 10);
		rtc_time.year = (rtc_time.year & 0x0F) + ((rtc_time.year / 16) * 10);
	}

	return rtc_time;
}