#include "special.h"

static char en_chars_lower[] = {
	0,    27,  '1', '2', '3',  '4', '5', '6', '7',  '8', /* 9 */
	'9',  '0', '-', '=', '\b',                           /* Backspace */
	'\t',                                                /* Tab */
	'q',  'w', 'e', 'r',                                 /* 19 */
	't',  'y', 'u', 'i', 'o',  'p', '[', ']', '\n',      /* Enter */
	0,                                                   /* Control */
	'a',  's', 'd', 'f', 'g',  'h', 'j', 'k', 'l',  ';', /* 39 */
	'\'', '`', 0,                                        /* Left shift */
	'\\', 'z', 'x', 'c', 'v',  'b', 'n',                 /* 49 */
	'm',  ',', '.', '/', 0,                              /* Right shift */
	'*',  0,                                             /* Alt */
	' ',                                                 /* Space bar */
	0,                                                   /* Caps lock */
	0,                                                   /* 59 - F1 */
	0,                                                   /* F2 */
	0,                                                   /* F3 */
	0,                                                   /* F4 */
	0,                                                   /* F5 */
	0,                                                   /* F6 */
	0,                                                   /* F7 */
	0,                                                   /* F8 */
	0,                                                   /* F9 */
	0,                                                   /* F10 */
	0,                                                   /* 69 - Num lock*/
	0,                                                   /* Scroll Lock */
	0,                                                   /* Home */
	0,                                                   /* Up Arrow */
	0,                                                   /* Page Up */
	'-',  0,                                             /* Left Arrow */
	0,    0,                                             /* Right Arrow */
	'+',  0,                                             /* 79 - End */
	0,                                                   /* Down Arrow */
	0,                                                   /* Page Down */
	0,                                                   /* Insert */
	0,                                                   /* Delete */
	0,    0,   0,   0,                                   /* F11 */
	0,                                                   /* F12 */
	0,                                                   /* NULL */
};

static char en_chars_shifted[] = {
	0,    27,  '!', '@', '#',  '$', '%', '^', '&',  '*', /* 9 */
	'(',  ')', '_', '+', '\b',                           /* Backspace */
	'\t',                                                /* Tab */
	'Q',  'W', 'E', 'R',                                 /* 19 */
	'T',  'Y', 'U', 'I', 'O',  'P', '{', '}', '\n',      /* Enter */
	0,                                                   /* 29   - Control */
	'A',  'S', 'D', 'F', 'G',  'H', 'J', 'K', 'L',  ':', /* 39 */
	'"',  '~', 0,                                        /* Left shift */
	'|',  'Z', 'X', 'C', 'V',  'B', 'N',                 /* 49 */
	'M',  '<', '>', '?', 0,                              /* Right shift */
	'*',  0,                                             /* Alt */
	' ',                                                 /* Space bar */
	0,                                                   /* Caps lock */
	0,                                                   /* 59 - F1 */
	0,                                                   /* F2 */
	0,                                                   /* F3 */
	0,                                                   /* F4 */
	0,                                                   /* F5 */
	0,                                                   /* F6 */
	0,                                                   /* F7 */
	0,                                                   /* F8 */
	0,                                                   /* F9 */
	0,                                                   /* F10 */
	0,                                                   /* 69 - Num lock*/
	0,                                                   /* Scroll Lock */
	0,                                                   /* Home */
	0,                                                   /* Up Arrow */
	0,                                                   /* Page Up */
	'-',  0,                                             /* Left Arrow */
	0,    0,                                             /* Right Arrow */
	'+',  0,                                             /* 79 - End */
	0,                                                   /* Down Arrow */
	0,                                                   /* Page Down */
	0,                                                   /* Insert */
	0,                                                   /* Delete */
	0,    0,   0,   0,                                   /* F11 */
	0,                                                   /* F12 */
	0,                                                   /* NULL */
};

static char ru_chars_lower[] = {
	0,    27,  '1', '2', '3',  '4', '5', '6', '7',  '8', /* 9 */
	'9',  '0', '-', '=', '\b',                           /* Backspace */
	'\t',                                                /* Tab */
	'й',  'ц', 'у', 'к',                                 /* 19 */
	'е',  'н', 'г', 'ш', 'щ',  'з', 'х', 'ъ', '\n',      /* Enter */
	0,                                                   /* Control */
	'ф',  'ы', 'в', 'а', 'п',  'р', 'о', 'л', 'д',  'ж', /* 39 */
	'э',  'ё', 0,                                        /* Left shift */
	'\\', 'я', 'ч', 'с', 'м',  'и', 'т',                 /* 49 */
	'ь',  'б', 'ю', '.', 0,                              /* Right shift */
	'*',  0,                                             /* Alt */
	' ',                                                 /* Space bar */
	0,                                                   /* Caps lock */
	0,                                                   /* 59 - F1 */
	0,                                                   /* F2 */
	0,                                                   /* F3 */
	0,                                                   /* F4 */
	0,                                                   /* F5 */
	0,                                                   /* F6 */
	0,                                                   /* F7 */
	0,                                                   /* F8 */
	0,                                                   /* F9 */
	0,                                                   /* F10 */
	0,                                                   /* 69 - Num lock*/
	0,                                                   /* Scroll Lock */
	0,                                                   /* Home */
	0,                                                   /* Up Arrow */
	0,                                                   /* Page Up */
	'-',  0,                                             /* Left Arrow */
	0,    0,                                             /* Right Arrow */
	'+',  0,                                             /* 79 - End */
	0,                                                   /* Down Arrow */
	0,                                                   /* Page Down */
	0,                                                   /* Insert */
	0,                                                   /* Delete */
	0,    0,   0,   0,                                   /* F11 */
	0,                                                   /* F12 */
	0,                                                   /* NULL */
};

static char ru_chars_shifted[] = {
	0,    27,  '!', '\"', '№',  ';', '%', ':', '?',  '*', /* 9 */
	'(',  ')', '_', '+',  '\b',                           /* Backspace */
	'\t',                                                 /* Tab */
	'Й',  'Ц', 'У', 'К',                                  /* 19 */
	'Е',  'Н', 'Г', 'Ш',  'Щ',  'З', 'Х', 'Ъ', '\n',      /* Enter */
	0,                                                    /* 29   - Control */
	'Ф',  'Ы', 'В', 'А',  'П',  'Р', 'О', 'Л', 'Д',  'Ж', /* 39 */
	'Э',  'Ё', 0,                                         /* Left shift */
	'\\', 'Я', 'Ч', 'С',  'М',  'И', 'Т',                 /* 49 */
	'Ь',  'Б', 'Ю', ',',  0,                              /* Right shift */
	'*',  0,                                              /* Alt */
	' ',                                                  /* Space bar */
	0,                                                    /* Caps lock */
	0,                                                    /* 59 - F1 */
	0,                                                    /* F2 */
	0,                                                    /* F3 */
	0,                                                    /* F4 */
	0,                                                    /* F5 */
	0,                                                    /* F6 */
	0,                                                    /* F7 */
	0,                                                    /* F8 */
	0,                                                    /* F9 */
	0,                                                    /* F10 */
	0,                                                    /* 69 - Num lock*/
	0,                                                    /* Scroll Lock */
	0,                                                    /* Home */
	0,                                                    /* Up Arrow */
	0,                                                    /* Page Up */
	'-',  0,                                              /* Left Arrow */
	0,    0,                                              /* Right Arrow */
	'+',  0,                                              /* 79 - End */
	0,                                                    /* Down Arrow */
	0,                                                    /* Page Down */
	0,                                                    /* Insert */
	0,                                                    /* Delete */
	0,    0,   0,   0,                                    /* F11 */
	0,                                                    /* F12 */
	0,                                                    /* NULL */
};