#include "kernel/pmm/pmm.h"
#include "kernel/vmm/vmm.h"
#include "arch/vmm_hal.h"

struct vmm_context kernel_vmm_context;

void 
vmm_init(struct boot_info* b)
{
	arch_vmm_init(b, &kernel_vmm_context);
}

uint8_t
vmm_create_context(struct vmm_context* res)
{
	return arch_vmm_create_context(res);
}

ptr_t
vmm_map_core(physical_address_t base, uint32_t len)
{
	return arch_vmm_map_core(base, len);
}

void
vmm_map(struct vmm_context* vmm, ptr_t vaddr, physical_address_t paddr, uint32_t flags)
{
	arch_vmm_map(vmm, vaddr, paddr, flags);
}

void
vmm_unmap(struct vmm_context* vmm, ptr_t vaddr)
{
	arch_vmm_unmap(vmm, vaddr);
}

static ptr_t
vmm_find_free_page()
{

	struct vmm_region* region = kernel_vmm_context.region_map;

	while (region != 0 && region->flags != VMM_REGION_USABLE && region->length >= 4096) {
		region = region->next;
	}

	return region == 0 ? 0 : region->start_address;	
}

ptr_t
vmm_alloc_page(uint32_t flags)
{
	physical_address_t phy_addr = pmm_alloc_page();

	ptr_t virt_addr	= vmm_find_free_page();

	vmm_map(&kernel_vmm_context, virt_addr, phy_addr, flags);
	
	return virt_addr;
}

void
vmm_free_page(ptr_t p)
{
	physical_address_t phy = arch_vmm_virt_to_phys(p);
	
	arch_vmm_unmap(p);
	
	pmm_free_page(phy);
}

uint32_t
vmm_get_page(ptr_t p)
{
	return (uint32_t)(p/4096);
}
