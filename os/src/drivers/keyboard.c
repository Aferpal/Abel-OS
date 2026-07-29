#include "drivers/keyboard.h"
#include "kernel/interrupt/irq.h"
#include "drivers/ps2_controller.h"
#include "kernel/printk.h"
#include "kernel/input/input.h"

#define BLOQ_MAY 0x3a
#define R_SHFT 0x2a
#define CTRL 0x1d
#define ALT 0x38
#define EXTENDED_START 0xe0
#define L_SHFT 0x36

static const keyboard_input_key_e scancode_to_key_value_s1[128] = {
	KEY_UNKNOWN, KEY_ESC, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_LAYOUT_01, KEY_LAYOUT_02, KEY_DEL,
	KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LAYOUT_03, KEY_LAYOUT_04, KEY_ENTER,
	KEY_L_CTRL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_LAYOUT_05, KEY_LAYOUT_06, KEY_LAYOUT_07,
	KEY_L_SHFT, KEY_BACKSLASH, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_LAYOUT_08, KEY_R_SHFT, 
	KEY_KP_STAR, KEY_L_ALT, KEY_SPACE, KEY_CAPS, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
	KEY_NUMLOCK, KEY_SCROLL_LOCK, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_4, KEY_KP_5, KEY_KP_6, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_0, KEY_KP_DOT,
	KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_F11, KEY_F12, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN
};

static const keyboard_input_key_e extended_scancode_to_ascii_s1[128] = {
	KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, 
	KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_PREV_TRACK, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, 
	KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_NEXT_TRACK, KEY_UNKNOWN, KEY_UNKNOWN, KEY_KP_ENTER, KEY_R_CTRL, 
	KEY_UNKNOWN, KEY_UNKNOWN, KEY_MUTE, KEY_CALCULATOR, KEY_PLAY, KEY_UNKNOWN, KEY_STOP, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, 
	KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_VOL_DOWN, KEY_UNKNOWN, KEY_VOL_UP, KEY_UNKNOWN, 
	KEY_WWW_HOME, KEY_UNKNOWN, KEY_UNKNOWN, KEY_KP_SLASH, KEY_UNKNOWN, KEY_UNKNOWN, KEY_ALT_GR, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, 
	KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, 
	KEY_UNKNOWN, KEY_HOME, KEY_ARROW_UP, KEY_PAGE_UP, KEY_UNKNOWN, KEY_ARROW_LEFT, KEY_UNKNOWN, KEY_ARROW_RIGHT, KEY_UNKNOWN, KEY_END, 
	KEY_ARROW_DOWN, KEY_PAGE_DOWN, KEY_INSERT, KEY_SUPR, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
	KEY_UNKNOWN, KEY_LEFT_GUI, KEY_RIGHT_GUI, KEY_APPS, KEY_POWER, KEY_SLEEP, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_WAKE, 
	KEY_UNKNOWN, KEY_WWW_SEARCH, KEY_WWW_FAVS, KEY_WWW_REFRESH, KEY_WWW_STOP, KET_WWW_FORWARD, KEY_WWW_BACK, KEY_MY_COMP, KEY_EMAIL, KEY_MEDIA_SELECT
};

static uint16_t extended = 0;

int  
ps2_keyboard_parse(uint8_t scancode, keyboard_input_key_e * key, keyboard_input_type_e * type)
{
	if (scancode == EXTENDED_START) {
		extended = 1;
		return 0;
	}

	if (extended == 0) {

		if (scancode >= 0x80) {
			*key = scancode_to_key_value_s1[scancode - 0x80];
			*type = KEY_RELEASE;
		} else {
			*key = scancode_to_key_value_s1[scancode];
			*type = KEY_PRESS;
		}
	
	}

	if (extended == 1) {

		extended = 0;

		if (scancode >= 0x80) {
			*key = extended_scancode_to_ascii_s1[scancode - 0x80];
			*type = KEY_RELEASE;
		} else {
			*key = extended_scancode_to_ascii_s1[scancode];
			*type = KEY_PRESS;
		}

	}

	return 1;

}

static void
ps2_keyboard_irq_handler(struct interrupt_frame* frame)
{
	uint8_t scancode = ps2_read_data();

	keyboard_input_key_e key;
	keyboard_input_type_e type;

	// ready to send
	int rts = ps2_keyboard_parse(scancode, &key, &type);

	if (rts == 1) {
		input_event_key_publish(key, type);
	}

}

void
ps2_keyboard_init()
{
	register_irq_handler(IRQ_KEYBOARD, ps2_keyboard_irq_handler);
}
