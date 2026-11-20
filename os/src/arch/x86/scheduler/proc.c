#include "kernel/scheduler/scheduler.h"
#include "arch/x86/scheduler/types.h"

extern void arch_userspace_trampoline();

void* 
arch_proc_prepare_user_stack(void* kstack, ptr_t code_start, ptr_t user_stack)
{
	struct jump_to_user_frame* trampoline_frame = (struct jump_to_user_frame*)((ptr_t)kstack - sizeof(struct jump_to_user_frame));

	trampoline_frame->ss = (4 * 8) | 3;
	trampoline_frame->sp = user_stack;
	trampoline_frame->eflags = 0x0202;
	trampoline_frame->cs = (3 * 8) | 3;
	trampoline_frame->entry_ip = code_start;

	struct sched_frame* frame  = (struct sched_frame*)((ptr_t)trampoline_frame - sizeof(struct sched_frame) + sizeof(uint32_t));
	frame->edi = 0;
	frame->esi = 0;
	frame->ebx = 0;
	frame->ebp = 0;
	frame->eflags = (uint32_t)0x00000002;
	frame->eip = (uint32_t)arch_userspace_trampoline;
	return frame;
}


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
