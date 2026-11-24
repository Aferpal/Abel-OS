#ifndef _INTERRUPT_FRAME_H_
#define _INTERRUPT_FRAME_H_
#include "stdint.h"
typedef struct interrupt_frame{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t int_number;
    uint32_t error_code;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;

} interrupt_frame_t;

#endif
