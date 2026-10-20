#ifndef _SCHEDULER_TYPES_H_
#define _SCHEDULER_TYPES_H_

#include "stdint.h"

typedef uint32_t pid_t;

enum p_status {
	READY = 0,
	RUNNING = 1,
	BLOCKED = 2
};

enum p_flags {
	PROC_OK = 0,
	PROC_EXIT = 1
};

struct pcb {
	pid_t id;
	uint32_t priority;
	uint32_t stack_ptr;
	uint16_t flags;
	uint16_t status;
	uint32_t runtime;
	uint32_t cr3;
};

struct sched_ops {
	uint8_t      (*init)(void);
	uint8_t      (*enqueue_proc)(struct pcb*);
	uint8_t      (*dequeue_proc)(pid_t);
	struct pcb*  (*get_next_proc)(struct pcb*);
	uint8_t      (*tick)(struct pcb*);
};

#endif
