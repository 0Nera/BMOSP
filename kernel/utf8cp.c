#include <mem.h>
#include <tool.h>

#define NONS (0x98)
#define SKIP_OR_NOT (dest != NONS)
#define M(W40, W45, W201) ((((W40)-0x80) << 10) | (((W45) - (W40)) << 5) | ((W201)-0x80))

unsigned int utf8_2_win1251(const char *utf8, char *win) {
	unsigned int dest, p, l1, l2, l3, inc, i, j, b1, b2, b3;
	const unsigned short AR[16] = {
		M(NONS, NONS, 0x86), M(0xA8, 0xB8, 0x87), M(0x80, 0x90, 0x95), M(0x81, 0x83, 0x96),
		M(0xAA, 0xBA, 0x97), M(0xBD, 0xBE, NONS), M(0xB2, 0xB3, NONS), M(0xAF, 0xBF, NONS),
		M(0xA3, 0xBC, 0x91), M(0x8A, 0x9A, 0x92), M(0x8C, 0x9C, 0x82), M(0x8E, 0x9E, NONS),
		M(0x8D, 0x9D, 0x93), M(NONS, NONS, 0x94), M(0xA1, 0xA2, 0x84), M(0x8F, 0x9F, NONS)
	};

	for (i = 0, j = 0; utf8[i] != 0; i += inc) {
		b1 = utf8[i];
		b2 = utf8[i + 1];
		b3 = utf8[i + 2];

		// Utf8 переводим в Unicode.
		inc = (0xE5000000u >> (((b1) >> 4) << 1)) & 0x3;
		p = ((((b1) << 12) + (((b2)&0x3F) << 6) + ((b3)&0x3F)) & (0x7FFFF >> inc)) >>
		    (((0xC5FFAAAAu >> (((b1) >> 4) << 1)) & 0x3) * 6);

		// Добавляем все остающиеся на месте.
		dest = (((inc != 0) & (((p >> 5) != 0x5) | (0xF71C852E >> b2))) - 1) & p;
		inc++;

		// Добавляем русские буквы кроме ё и Ё.
		dest += ((((p - 0x10) >> 6) != 0x10) - 1) & (p - 0x350);

		// Добавляем символы из диапазонов: 0x401-0x40F, 0x451-0x45F, 0x2013-0x2022.
		l1 = ((p >> 4) != 0x40) - 1;
		l2 = ((p >> 4) != 0x45) - 1;
		l3 = (((p - 3) >> 4) != 0x201) - 1;
		dest += ((((l2 & (AR[p & 0xF] >> 5)) | (l3 & AR[p & 0xF])) & 0x1F) + ((l1 | l2) & (AR[p & 0xF] >> 10))) +
		        ((l1 | l2 | l3) & 0x80);

		// Добавляем оставшиеся.
		dest += (((p != 0x490) - 1) & 0xA5) | (((p != 0x491) - 1) & 0xB4) | (((p != 0x2026) - 1) & 0x85) |
		        (((p != 0x2030) - 1) & 0x89) | (((p != 0x2039) - 1) & 0x8B) | (((p != 0x203A) - 1) & 0x9B) |
		        (((p != 0x20AC) - 1) & 0x88) | (((p != 0x2116) - 1) & 0xB9) | (((p != 0x2122) - 1) & 0x99);

		// Отличаем настоящий 0 от просто отсутствующих в win 1251 символов.
		dest += (((b1 == 0) | (dest != 0)) - 1) & NONS;

		win[j] = dest;
		j += SKIP_OR_NOT;
	}
	win[j] = 0;
	return j;
}

#undef M
#undef NONS
#undef SKIP_OR_NOT

char *utf8cp(char *str) {
	size_t len = tool_strlen(str);
	char *result = (char *)mem_alloc(len + 1);
	if (result) { utf8_2_win1251(str, result); }
	return result;
}