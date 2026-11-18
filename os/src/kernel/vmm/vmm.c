#include "kernel/pmm/pmm.h"
#include "kernel/vmm/vmm.h"
#include "arch/vmm_hal.h"
#include "kernel/vmm/regions.h"

struct vmm_context kernel_vmm_context;

void 
vmm_init(struct boot_info* b)
{
	vmm_region_pool_init();
	arch_vmm_init(b, &kernel_vmm_context);
}

uint8_t
vmm_create_context(struct vmm_context* res)
{
	return arch_vmm_create_context(&kernel_vmm_context, res);
}

ptr_t
vmm_map_core(physical_address_t base, uint32_t len)
{
	ptr_t selected_address = 0;
	uint32_t n_pages = ((len + (4095)) / 4096);
	uint32_t normalised_len = n_pages * 4096;

	struct vmm_region* region = vmm_find_free_range(kernel_vmm_context.region_map, normalised_len);

	if (!region) {
		return 0;
	}

	selected_address = region->start_address;

	arch_vmm_map(&kernel_vmm_context, selected_address, base, n_pages, VMM_PROT_KERNEL | VMM_PROT_WRITE);

	vmm_reserve_io_range(kernel_vmm_context.region_map, region->start_address, normalised_len, VMM_NOALLOC);

	return selected_address;
}

ptr_t
vmm_map_io(physical_address_t base, uint32_t len)
{
	ptr_t selected_address = 0;
	uint32_t n_pages = ((len + (4095)) / 4096);
	uint32_t normalised_len = n_pages * 4096;

	struct vmm_region* region = vmm_find_free_range(kernel_vmm_context.region_map, normalised_len);

	if (!region) {
		return 0;
	}

	selected_address = region->start_address;

	arch_vmm_map(&kernel_vmm_context, selected_address, base, n_pages, VMM_PROT_KERNEL | VMM_PROT_WRITE);

	vmm_reserve_io_range(kernel_vmm_context.region_map, region->start_address, normalised_len, 0);

	return selected_address;
}

void
vmm_map(struct vmm_context* vmm, ptr_t vaddr, physical_address_t paddr, uint32_t flags)
{

	arch_vmm_map(vmm, vaddr, paddr, 1, flags);

	vmm_reserve_range(kernel_vmm_context.region_map, vaddr, 4096, 0);

}

void
vmm_map_noalloc(struct vmm_context* vmm, ptr_t vaddr, physical_address_t paddr, uint32_t flags)
{

	arch_vmm_map(vmm, vaddr, paddr, 1, flags);

	vmm_reserve_range(kernel_vmm_context.region_map, vaddr, 4096, VMM_NOALLOC);

}

void
vmm_unmap(struct vmm_context* vmm, ptr_t vaddr)
{
	struct vmm_region* region = get_region_from_ptr(vmm->region_map, vaddr);
	arch_vmm_unmap(&kernel_vmm_context, vaddr);

	vmm_free_range(region, vaddr, 4096, 0);
}

ptr_t
vmm_alloc_page(uint32_t flags)
{
	physical_address_t phy_addr = pmm_alloc_page();

	if (phy_addr == 0) {
		return 0;
	}

	struct vmm_region* region = vmm_find_free_range(kernel_vmm_context.region_map, 4096);

	if (region == 0) {
		pmm_free_page(phy_addr);
		return 0;
	}

	ptr_t return_address = region->start_address;

	vmm_map(&kernel_vmm_context, region->start_address, phy_addr, flags);

	return return_address;
}

ptr_t
vmm_alloc_page_noalloc(uint32_t flags)
{
	physical_address_t phy_addr =pmm_alloc_page();

	if(!phy_addr) {
		return 0;
	}

	struct vmm_region* region = vmm_find_free_range(kernel_vmm_context.region_map, 4096);

	if (region == 0) {
		pmm_free_page(phy_addr);
		return 0;
	}

	ptr_t return_address = region->start_address;

	vmm_map_noalloc(&kernel_vmm_context, region->start_address, phy_addr, flags);

	return return_address;
}

void
vmm_free_page(ptr_t p)
{

	ptr_t page_aligned = vmm_get_page(p) * 4096;

	physical_address_t phy = arch_vmm_virt_to_phys(&kernel_vmm_context, page_aligned);
	
	vmm_unmap(&kernel_vmm_context, page_aligned);
	
	pmm_free_page(phy);
}

uint32_t
vmm_get_page(ptr_t p)
{
	return (uint32_t)(p/4096);
}

struct vmm_context* 
get_kernel_vmm()
{
	return &kernel_vmm_context;
}

void
vmm_print_map()
{

	struct vmm_region* regionp = kernel_vmm_context.region_map;
    printk("Printing region list!\n");
    while(regionp != 0) {
        printk("Region [%x, %x), %s, %s\n", 
            regionp->start_address, 
            regionp->start_address + regionp->length, 
            regionp->flags & VMM_REGION_USABLE ? "free" : "used",
            regionp->flags & VMM_REGION_IO ? "mmio": "kernel");
        regionp = regionp->next;
    }

}