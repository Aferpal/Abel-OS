#include "kernel/vmm/vmm.h"
#include "arch/vmm_hal.h"
/* creates an address space, that is in x86 a page directory */

void
vmm_init()
{
	
}

struct vmm_addr_space*
vmm_create_address_space()
{
	return arch_vmm_create_address_space();
}

/* maps a certain physical address into a virtual address in the addr space given */
void
vmm_map(struct vmm_addr_space* ad_space, ptr_t phys, ptr_t virt)
{
	arch_vmm_map(ad_space, phys, virt);
}

/* clears the virtual address in an address space */
void
vmm_unmap(struct vmm_addr_space* ad_space, ptr_t virt)
{
	arch_vmm_unmap(ad_space, virt);
}

