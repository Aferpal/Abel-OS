#include "kernel/scheduler/scheduler.h"

void* 
arch_proc_prepare_stack(void (*p_start)(void), void* stack_start)
{
	struct sched_frame* frame  = (struct sched_frame*)((uint8_t*)stack_start - sizeof(struct sched_frame));
	frame->edi = 0;
	frame->esi = 0;
	frame->ebx = 0;
	frame->ebp = 0;
	frame->eflags = (uint32_t)0x00000202;
	frame->eip = (uint32_t)p_start;
	frame->exit = (uint32_t)scheduler_exit_current;

	return frame;
}
