#include "kernel/pmm/pmm.h"
#include "kernel/vmm/vmm.h"
#include "arch/vmm_hal.h"

void 
vmm_init(struct boot_info* b)
{
	arch_vmm_init(b);
}

ptr_t
vmm_map_core(physical_address_t base, uint32_t len)
{
	return arch_vmm_map_core(base, len);
}

void
vmm_map(struct vmm_addr_space* addr_space, ptr_t vaddr, physical_address_t paddr)
{
	arch_vmm_map(addr_space, vaddr, paddr);
}

ptr_t
vmm_alloc_page()
{
	physical_address_t phy_addr = pmm_alloc_page();

	ptr_t virt_addr	= arch_vmm_find_unused_page_kernel();

	vmm_map(arch_vmm_current_addr_space(), virt_addr, phy_addr);
	
	return virt_addr;
}

void
vmm_free_page(ptr_t p)
{
	physical_address_t phy = arch_vmm_virt_to_phys(p);
	
	arch_vmm_dealloc_page(p);
	
	pmm_free_page(phy);
}

uint32_t
vmm_get_page(ptr_t p)
{
	return (uint32_t)(p/4096);
}
