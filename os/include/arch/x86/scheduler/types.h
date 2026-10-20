#ifndef _ARCH_X86_SCHEDULER_TYPES_H_
#define _ARCH_X86_SCHEDULER_TYPES_H_

#include "stdint.h"

struct sched_frame {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebx;
	uint32_t ebp;
	uint32_t eflags;
	uint32_t eip; 
	uint32_t exit;
};

#endif