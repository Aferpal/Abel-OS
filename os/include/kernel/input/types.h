#ifndef _INPUT_TYPES_H_
#define _INPUT_TYPES_H_

typedef enum input_evid input_evid_e;

typedef enum keyboard_input_key keyboard_input_key_e;

typedef enum keyboard_input_type keyboard_input_type_e;

typedef struct input_event input_event_t;

enum input_evid {
    EV_KEY
};


enum keyboard_input_key {
    KEY_UNKNOWN, KEY_ESC, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, 
    KEY_9, KEY_0, KEY_LAYOUT_01, KEY_LAYOUT_02, KEY_DEL, KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, 
    KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LAYOUT_03, KEY_LAYOUT_04, KEY_ENTER, KEY_L_CTRL, 
    KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_LAYOUT_05, 
    KEY_LAYOUT_06, KEY_LAYOUT_07, KEY_L_SHFT, KEY_BACKSLASH, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, 
    KEY_M, KEY_COMMA, KEY_DOT, KEY_LAYOUT_08, KEY_R_SHFT, KEY_KP_STAR, KEY_L_ALT, KEY_SPACE, KEY_CAPS, KEY_F1, 
    KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_NUMLOCK, 
    KEY_SCROLL_LOCK, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_4, KEY_KP_5, KEY_KP_6, KEY_KP_1, KEY_KP_2, KEY_KP_3, 
    KEY_KP_0, KEY_KP_DOT, KEY_F11, KEY_F12, KEY_PREV_TRACK, KEY_NEXT_TRACK, KEY_KP_ENTER, KEY_R_CTRL, KEY_MUTE, KEY_CALCULATOR, 
    KEY_PLAY, KEY_STOP, KEY_VOL_DOWN, KEY_VOL_UP, KEY_WWW_HOME, KEY_KP_SLASH, KEY_ALT_GR, KEY_HOME, KEY_ARROW_UP, KEY_PAGE_UP, 
    KEY_ARROW_LEFT, KEY_ARROW_RIGHT, KEY_END, KEY_ARROW_DOWN, KEY_PAGE_DOWN, KEY_INSERT, KEY_SUPR, KEY_LEFT_GUI, KEY_RIGHT_GUI, KEY_APPS, 
    KEY_POWER, KEY_SLEEP, KEY_WAKE, KEY_WWW_SEARCH, KEY_WWW_FAVS, KEY_WWW_REFRESH, KEY_WWW_STOP, KET_WWW_FORWARD, KEY_WWW_BACK, KEY_MY_COMP, 
    KEY_EMAIL, KEY_MEDIA_SELECT
};

enum keyboard_input_type {
    KEY_PRESS,
    KEY_RELEASE
};

struct keyboard_input_event {
    keyboard_input_key_e value;
    keyboard_input_type_e type;
};

struct input_event {
    input_evid_e evid;
    union {
        struct keyboard_input_event key_evt;
    };
};


const char* get_key_name(keyboard_input_key_e);

#endif