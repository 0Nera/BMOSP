#include <system.h>

#define MAX_KEY_BUFFER_SIZE 1
#define NORMAL_STATE 0
#define PREFIX_STATE 1

typedef struct {
	uint8_t code;
	bool ctrl_pressed;
	bool alt_pressed;
	bool shift_pressed;
	bool sys_menu_pressed;
} key_event_t;

typedef struct {
	uint8_t code;
	uint8_t c;
} keyboard_char_t;