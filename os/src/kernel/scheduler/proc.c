#include "kernel/scheduler/scheduler.h"
#include "arch/scheduler_hal.h"
#include "kernel/vmm/vmm.h"
#include "kernel/scheduler/proc.h"

uint8_t
proc_create_process(void (*p_start)(void), uint32_t priority, struct pcb* res)
{

	// 1. Create a page directory
	struct vmm_addr_space* p_layout;
	int rc = vmm_create_addr_space(&p_layout);

	void* stack_end = (void*)vmm_alloc_page(VMM_PROT_KERNEL | VMM_PROT_WRITE);
	
	if (stack_end == 0) {
		return 1;
	}

	// stacks work the other way around so switch to stack start
	
	void* stack_start = (stack_end + 4096);

	void* stack_top = arch_proc_prepare_stack(p_start, stack_start); // inside this fun the needed preparation is done 

	vmm_map(p_layout, stack_end, PROC_USER_STACK_START_ADDRESS, VMM_PROT_WRITE | VMM_PROT_USER);

	res->id = 0;
	res->priority = priority;
	res->stack_ptr = (uint32_t)stack_top;
	res->flags = READY;
	res->runtime = 0;
	res->cr3 = ...;
	return 0;
}

uint8_t
proc_create_kthread(void (*p_start)(void), struct pcb* res)
{
	void* stack_end = (void*)vmm_alloc_page(VMM_PROT_WRITE | VMM_PROT_KERNEL);

	if (stack_end == 0) {
		return 1;
	}

	// stacks work the other way around so switch to stack start
	
	void* stack_start = (stack_end + 4096);

	void* stack_top = arch_proc_prepare_stack(p_start, stack_start); // inside this fun the needed preparation is done 

	res->id = 0;
	res->priority = 100;
	res->stack_ptr = (uint32_t)stack_top;
	res->flags = READY;
	res->runtime = 0;
	res->cr3 = (...);
	return 0;

}
