#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "stdint.h"
#include "kernel/interrupt/types.h"

void init_int_subsystem();

void enable_interrupts();

void disable_interrupts();

#endif
