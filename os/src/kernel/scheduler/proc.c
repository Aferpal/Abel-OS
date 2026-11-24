#include "kernel/scheduler/scheduler.h"
#include "arch/scheduler_hal.h"
#include "kernel/pmm/pmm.h"
#include "kernel/vmm/vmm.h"
#include "arch/vmm_hal.h"
#include "kernel/scheduler/proc.h"
#include "kernel/kalloc/kalloc.h"
#include "kernel/printk.h"

uint8_t
proc_create_process(void (*p_start)(void), uint32_t priority, struct pcb* res)
{

	// 1. Create a memory context
	struct vmm_context* user_vmm_context = kalloc(sizeof(struct vmm_context));
	uint8_t rc = vmm_create_context(user_vmm_context);

	/* get USER STACK s*/
	physical_address_t stack_end = pmm_alloc_page();
	
	if (stack_end == 0) {
		return 1;
	}

	/* we map it to user */
	vmm_map(user_vmm_context, PROC_USER_STACK_START_ADDRESS, stack_end, VMM_PROT_WRITE | VMM_PROT_USER);
		
	/* get and map a page for code */
	vmm_map(user_vmm_context, PROC_USER_CODE_START_ADDRESS, arch_vmm_virt_to_phys(get_kernel_vmm(), (ptr_t)p_start), VMM_PROT_EXEC | VMM_PROT_USER);

	/* we also create a kernel stack */
	ptr_t kstack_end = vmm_alloc_page(VMM_PROT_KERNEL | VMM_PROT_WRITE);
	void* kstack_start = (void*)(kstack_end + 4096);

	void* kstack_top = arch_proc_prepare_user_stack(kstack_start, PROC_USER_CODE_START_ADDRESS, PROC_USER_STACK_END_ADDRESS);

	/* after mapping the stacks we need to map the code itself */

	res->id = 0;
	res->priority = priority;
	res->stack_ptr = (uint32_t)kstack_top;
	res->flags = READY;
	res->runtime = 0;
	res->kstack_ptr = (uint32_t)kstack_top;
	res->vmm = user_vmm_context;
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
	res->kstack_ptr = 0; /* this is used to show that a pcb is a kthread rather than a proc */
	res->flags = READY;
	res->runtime = 0;
	res->vmm = get_kernel_vmm();
	return 0;

}
