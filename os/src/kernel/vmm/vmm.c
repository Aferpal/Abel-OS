#include "kernel/vmm/vmm.h"
#include "kernel/arch/vmm_hal.h"

void 
vmm_init(struct boot_info* b)
{
	// we have to modify our very early and well known
	// base page directory
	// we know for well that right after kernel_end we 
	// have our page directory. After that we have the identity
	// ( it will be used for swap later on ), a page for core data
	// ( this one will include IO, pmm bitmap, etc... ) and a page 
	// for the kernel code / data. This last one is the only one
	// we will have ready. The other two will be left linked to the
	// page directory but cleaned up
	
	// this is like this in x86 so we delegate this job to the hal ( arch )
	arch_vmm_init(b);
}
