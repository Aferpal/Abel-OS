#ifndef _INTERRUPT_TYPES_H_
#define _INTERRUPT_TYPES_H_

#include "arch/x86/interrupt/frame.h"

typedef struct interrupt_frame interrupt_frame_t;
typedef interrupt_frame_t cpu_frame_t;

typedef void (*interrupt_handler_t)(struct interrupt_frame*);

#endif