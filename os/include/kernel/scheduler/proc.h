#ifndef _SCHEDULER_PROC_H_
#define _SCHEDULER_PROC_H_

#include "stdint.h"
#include "kernel/scheduler/scheduler.h"

#define PROC_USER_CODE_START_ADDRESS  0x1000
#define PROC_USER_STACK_START_ADDRESS 0xbffe0000
#define PROC_USER_DEFAULT_STACK_SIZE  4096
#define PROC_USER_STACK_END_ADDRESS   (PROC_USER_STACK_START_ADDRESS + PROC_USER_DEFAULT_STACK_SIZE)

uint8_t proc_create_process(void (*p_start)(void), uint32_t priority, struct pcb* res);

uint8_t proc_create_kthread(void (*p_start)(void), struct pcb* res);

#endif
