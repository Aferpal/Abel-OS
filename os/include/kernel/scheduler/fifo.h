#ifndef _SCHEDULER_FIFO_H_
#define _SCHEDULER_FIFO_H_

#include "kernel/scheduler/types.h"

uint8_t sched_fifo_init(void);

uint8_t sched_fifo_enqueue(struct pcb*);

uint8_t sched_fifo_dequeue(pid_t);

struct pcb* sched_fifo_next(struct pcb*);

uint8_t sched_fifo_tick(struct pcb*);

#endif
