#include "kernel/scheduler/scheduler.h"
#include "arch/scheduler_hal.h"
#include "kernel/pmm/pmm.h"

uint32_t
proc_create_process(void (*p_start)(void))
{
	void* stack_end = pmm_alloc_page();
	
	if (stack_end == 0) {
		return 0;
	}

	// stacks work the other way around so switch to stack start
	
	void* stack_start = (stack_end + 4096);

	void* stack_top = arch_proc_prepare_stack(p_start, stack_start); // inside this fun the needed preparation is done 

	return (uint32_t)stack_top;
}
