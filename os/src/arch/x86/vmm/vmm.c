#include "arch/vmm_hal.h"
#include "arch/x86/vmm/types.h"
#include "kernel/pmm/pmm.h"

struct vmm_core_data vmm_data;

extern uint8_t* kernel_start;
extern uint8_t* kernel_end;
extern uint8_t* kernel_virt_start;
extern uint8_t* kernel_phys_start;

extern void reload_page_directory();

void
arch_vmm_init(struct boot_info* b)
{
	// :)
	/* First of all we want to initialize our core data
	 * That is get the physical addresses of our minimun
	 * needed pages
	 */
	uint32_t k_page_dir_addr = (physical_address_t)(&kernel_end 
	                            - &kernel_virt_start 
	                            + &kernel_phys_start);
	
	vmm_data.k_page_dir_addr = k_page_dir_addr;
	vmm_data.k_swap_page_table_addr = k_page_dir_addr + X86_PAGE_SIZE;
	vmm_data.k_io_page_table_addr = k_page_dir_addr + (2*X86_PAGE_SIZE);
	vmm_data.k_code_page_table_addr = k_page_dir_addr + (3*X86_PAGE_SIZE);

	uint32_t ks = (uint32_t)((uint32_t)(&kernel_end) - (uint32_t)(&kernel_start));
	uint32_t kernel_pages = ((ks + X86_PAGE_SIZE - 1)/X86_PAGE_SIZE);

	/* 
	 * After having our core data, we need to set the default, startup
	 * virtual memory layout. We are going to reuse all the pages we
	 * already allocated at the bootstrap.
	 *
	 * The Layout will be as follows:
	 * 0x00000000 - 0xC0000000 -> Free Memory, unmapped, no pts
	 * 0xC0000000 - 0xC0400000 -> IO mem range, unmapped but has pt
	 * 0xC0400000 - 0xC0800000 -> Kernel, mapped and loaded at 0x10000
	 *                         -> if kernel is larger than 4MB you did
	 *                         -> something wrong :)
	 * 0xC0800000 - 0xFF800000 -> Free memory, unmapped, no pts
	 * 0xFF800000 - 0xFFC00000 -> swap page, unmapped but has pt
	 * 0xFFC00000 - 0xFFFFFFFF -> Recursive mapping of page dir itself
	 *                         -> through this 4MB address range we will
	 *                         -> modify the kernel pages themselves
	 */

	/*
	 * if we were creating a new address space this could be easy but
	 * yk we must be carefull
	 */

	/* 
	 * First remove the Idenity paging ( set to 0 the first pd entry )
	 */
	struct vmm_addr_space* addr_space = (struct vmm_addr_space*)X86_CURRENT_ADDR_SPACE;

	addr_space->p_dir.pdes[0] = 0;
	
	/* We now remap the known physical address to the definitive vmem addr */
	
	addr_space->p_dir.pdes[X86_SWAP_PAGE_IDX] = 
		                        ((vmm_data.k_swap_page_table_addr 
	                                & X86_PDE_PT_MASK ) |
	                                X86_PAGE_FLAG_KERNEL);

	/* And fill it with 0s */
	struct page_table* swap_pt = &(addr_space->p_tables[X86_SWAP_PAGE_IDX]);
	for (int i = 0; i < X86_PAGE_N_ENTRIES; i++) {
		swap_pt->ptes[i] = X86_PAGE_NULL_ENTRY; 
	}

	/* At last we shrink the kernel mapped pages to the bare minimum */
	struct page_table* kernel_pt = &(addr_space->p_tables[X86_KERNEL_PAGE_IDX]);
	
	for (int i = kernel_pages; i < X86_PAGE_N_ENTRIES; i++) {
		kernel_pt->ptes[i] = X86_PAGE_NULL_ENTRY;
	}

	/* And thats it :) 
	 * now just update the cr3 register :p
	 */

	reload_page_directory();
}

static void
arch_vmm_alloc_page_table(struct vmm_addr_space* addr_space, uint32_t pdi)
{
	if (addr_space == 0) {
		return;
	}

	if (addr_space->p_dir.pdes[pdi] & X86_PAGE_FLAG_PRESENT) {
		// this page table already exists!
		return;
	}

	physical_address_t p_addr = pmm_alloc_page();

	addr_space->p_dir.pdes[pdi] = (p_addr | X86_PAGE_FLAG_KERNEL);

	/* theorically nothing else is to be done */

	/* we make sure that the change has been done */
	reload_page_directory(); 

	/* and fill with 0s the new pagem table */
	for (int i = 0; i < X86_PAGE_N_ENTRIES; i++) {
		addr_space->p_tables[pdi].ptes[i] = X86_PAGE_NULL_ENTRY;
	}
}

ptr_t
arch_vmm_find_unused_page_kernel()
{
	struct vmm_addr_space* asp = (struct vmm_addr_space*)X86_CURRENT_ADDR_SPACE;

	struct page_table* kernel_pt = &(asp->p_tables[X86_KERNEL_PAGE_IDX]);

	uint16_t i = 0;

	while ((i < X86_PAGE_N_ENTRIES) && (kernel_pt->ptes[i] != 0)) {
		i++;
	}

	if (i == X86_PAGE_N_ENTRIES) {
		// in this case we are pretty joever, should probably
		// get new page tables for kernel
		return 0;
	}

	/* now what is the address of the page table entry i in the kernel? */
	/* well kernel base address is known */
	uint32_t kstart = (uint32_t)(&kernel_virt_start);
	/* to that we need to add i*PAGE_SIZE */
	kstart = kstart + (i * X86_PAGE_SIZE);

	return (ptr_t)kstart;

}

ptr_t
arch_vmm_map_core(physical_address_t base, uint32_t len)
{
	/* calc number of needed */
	uint32_t n_pages = ((len + X86_PAGE_SIZE)/ X86_PAGE_SIZE );

	/* we need to allocate such a big mem block in the page table */
	struct vmm_addr_space* addr_space = (struct vmm_addr_space*)X86_CURRENT_ADDR_SPACE;

	struct page_table* core_pt = &(addr_space->p_tables[X86_IO_PAGE_IDX]);

	uint32_t found = 0;
	uint32_t i;
	for (i = 0; (i < X86_PAGE_N_ENTRIES) && (found < n_pages); i++) {
		
		if (core_pt->ptes[i] & X86_PAGE_FLAG_PRESENT) {
			found = 0;
		} else {
			found++;
		}
	}

	if (found != n_pages) {
		return 0;
	}

	i = i - n_pages;

	for (int j = 0; j < n_pages; j++) {
		core_pt->ptes[i+j] = 0;
		core_pt->ptes[i + j] = (base + (j*X86_PAGE_SIZE)) | X86_PAGE_FLAG_KERNEL;
	}

	/*
	 * now we should return the actual virtual address that corresponds to
	 * that pte 
	 */

	/* we know that IO page is in 0xC0000000 don't we? */
	ptr_t core_base = 0xC0000000;
	core_base = core_base + (X86_PAGE_SIZE * i);

	return core_base;
}

void
arch_vmm_dealloc_page(ptr_t p)
{
	uint32_t pdi = X86_ADDR_TO_PDI(p);
	uint32_t pti = X86_ADDR_TO_PTI(p);

	struct vmm_addr_space* cas = arch_vmm_current_addr_space();

	if (!(cas->p_dir.pdes[pdi] & X86_PAGE_FLAG_PRESENT)) {
		// if not present what tha hell
		return;
	}

	cas->p_tables[pdi].ptes[pti] = 0;

}

physical_address_t
arch_vmm_virt_to_phys(ptr_t p)
{
	uint32_t pdi = X86_ADDR_TO_PDI(p);
	uint32_t pti = X86_ADDR_TO_PTI(p);
	
	struct vmm_addr_space* cas = arch_vmm_current_addr_space();

	if (!(cas->p_dir.pdes[pdi] & X86_PAGE_FLAG_PRESENT)) {
		// if not present what tha hell
		return 0;
	}

	if (!(cas->p_tables[pdi].ptes[pti] & X86_PAGE_FLAG_PRESENT)) {
		return 0;
	}

	return (cas->p_tables[pdi].ptes[pti] & X86_PDE_PT_MASK); 
	
}

struct vmm_addr_space* 
arch_vmm_current_addr_space()
{
	return (struct vmm_addr_space*)X86_CURRENT_ADDR_SPACE;
}

void
arch_vmm_map(struct vmm_addr_space* addr_space, ptr_t v, physical_address_t p)
{
	uint32_t pdi = X86_ADDR_TO_PDI(v);
	uint32_t pti = X86_ADDR_TO_PTI(v);

	if (!(addr_space->p_dir.pdes[pdi] & X86_PAGE_FLAG_PRESENT)) {
		// in this case we need a new page table!!
		return;
	}

	if (addr_space->p_tables[pdi].ptes[pti] & X86_PAGE_FLAG_PRESENT) {
		// the page is already used!!
		return;
	}

	addr_space->p_tables[pdi].ptes[pti] = (p | X86_PAGE_FLAG_KERNEL);

	reload_page_directory();	
}
