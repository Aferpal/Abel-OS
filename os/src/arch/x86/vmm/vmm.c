#include "arch/x86/vmm/types.h"
#include "arch/vmm_hal.h"
#include "kernel/pmm/pmm.h"
#include "mem.h"

virtual_address_t 
phys_to_virtual(physical_address_t p_addr)
{
	/* if (!virt_activated) { */
		return (virtual_address_t)p_addr;
	/* } else { */
	/* 	do something? */
	/* } */
}

virtual_address_t
get_page_directory_ptr_from_phys_addr(physical_address_t addr)
{
	
}

struct vmm_addr_space*
arch_vmm_create_address_space()
{
	ptr_t phys_page_start_addr = pmm_alloc_page();

	if (phys_page_start_addr == 0) {
		return 0;
	}

	struct page_directory* pd = (struct page_directory*) phys_page_start_addr;

	//assert(sizeof(struct page_directory) == 4096);

	memset(0x00, (void*)pd, sizeof(struct page_directory));

	return (struct vmm_addr_space*)pd;
}

void
arch_vmm_map(struct vmm_addr_space* ad_space, physical_address_t phys, virtual_address_t virt)
{
	if (ad_space == 0) {
		return;
	}

	struct page_directory* pd = (struct page_directory*)ad_space;

	uint32_t page_directory_index = (virt & X86_VIRT_ADDR_PDI_MASK) >> X86_VIRT_ADDR_PDI_RSHFT;
	uint32_t page_table_index = (virt & X86_VIRT_ADDR_PTI_MASK) >> X86_VIRT_ADDR_PTI_RSHFT;

	/* if page is not present */
	if ((pd->pdes[page_directory_index].value & X86_PAGE_FLAG_PRESENT) == 0) {
		/* alloc page */
		physical_address_t n_pt = pmm_alloc_page();
		if (n_pt == 0) {
			return ;
		}
		
		/* init to all 0 */
		//assert(sizeof(struct page_table) == 4096);
		memset(0x00, (void*)n_pt, sizeof(struct page_table));
		
		/* set the entry and the flags */
		pd->pdes[page_directory_index].value = ((uint32_t)n_pt & X86_PDE_PT_MASK) | X86_PAGE_FLAG_KERNEL;
	}

	struct page_table* pt = (struct page_table*)(pd->pdes[page_directory_index].value & X86_PDE_PT_MASK);

	pt->ptes[page_table_index].value = (phys | X86_PAGE_FLAG_KERNEL);

}

void
arch_vmm_unmap(struct vmm_addr_space* ad_space, ptr_t virt)
{
	if (ad_space == 0) {
                return;
        }

        struct page_directory* pd = (struct page_directory*)ad_space;

        uint32_t page_directory_index = (virt & X86_VIRT_ADDR_PDI_MASK) >> X86_VIRT_ADDR_PDI_RSHFT;
        uint32_t page_table_index = (virt & X86_VIRT_ADDR_PTI_MASK) >> X86_VIRT_ADDR_PTI_RSHFT;

        /* if page is not present */
        if ((pd->pdes[page_directory_index].value & X86_PAGE_FLAG_PRESENT) == 0) {
        	return; // nothing to do here...
	}

        struct page_table* pt = (struct page_table*)(pd->pdes[page_directory_index].value & X86_PDE_PT_MASK);

        pt->ptes[page_table_index].value &= (~(X86_PAGE_FLAG_PRESENT));

}

