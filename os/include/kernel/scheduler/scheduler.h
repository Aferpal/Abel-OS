#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "kernel/scheduler/types.h"

void scheduler_init(struct sched_ops*);

void scheduler_add_process(uint32_t stack, uint32_t priority);

void scheduler_kill_process(pid_t);

void scheduler_schedule();

void scheduler_tick();

void scheduler_start();

void scheduler_exit_current();

#endif
