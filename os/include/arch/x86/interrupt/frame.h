#ifndef _INTERRUPT_FRAME_H_
#define _INTERRUPT_FRAME_H_

typedef struct interrupt_frame{
    int edi;
    int esi;
    int ebp;
    int esp;
    int ebx;
    int edx;
    int ecx;
    int eax;

    int int_number;
    int error_code;

    int eip;
    int cs;
    int eflags;

} interrupt_frame_t;

#endif
