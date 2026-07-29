#ifndef _INPUT_H_
#define _INPUT_H_

#include "kernel/input/types.h"
#include "stdint.h"


void input_event_publish(input_event_t*);

void input_event_key_publish(keyboard_input_key_e, keyboard_input_type_e);

void input_subsystem_init();

uint8_t input_event_pop(struct input_event*);

#endif