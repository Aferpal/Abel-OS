#include "stdint.h"
#include "arch/x86/vmm/types.h"
#include "arch/vmm_hal.h"
#include "kernel/pmm/pmm.h"
#include "kernel/kalloc/kalloc.h"
#include "kernel/vmm/defines.h"

extern uint8_t* kernel_start;
extern uint8_t* kernel_end;
extern uint8_t* kernel_virt_start;
extern uint8_t* kernel_phys_start;

static struct vmm_core_data vmm_data;
static struct vmm_arch_data arch_data_vmm;

static struct vmm_region user_range, io_range, kactive_range, k_range, rec_range;

extern void reload_page_directory();

extern uint32_t get_current_cr3();

void
arch_vmm_init(struct boot_info* b, struct vmm_context* out_context)
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
	vmm_data.k_swap_page_table_addr = k_page_dir_addr + X86_PAGE_SIZE; // old identity
	vmm_data.k_io_page_table_addr = k_page_dir_addr + (2*X86_PAGE_SIZE); // pre-reserved entry?
	vmm_data.k_code_page_table_addr = k_page_dir_addr + (3*X86_PAGE_SIZE); // kernel

	arch_data_vmm.cr3 = vmm_data.k_page_dir_addr;

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
	 * 0xFF800000 - 0xFFC00000 -> kernel page directory
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
	
	addr_space->p_dir.pdes[X86_KERNEL_PD_IDX] = 
		                        ((vmm_data.k_page_dir_addr 
	                                & X86_PDE_PT_MASK ) |
	                                X86_PAGE_FLAG_KERNEL);

	/* At last we shrink the kernel mapped pages to the bare minimum */
	struct page_table* kernel_pt = &(addr_space->p_tables[X86_KERNEL_PAGE_IDX]);
	
	for (int i = kernel_pages; i < X86_PAGE_N_ENTRIES; i++) {
		kernel_pt->ptes[i] = X86_PAGE_NULL_ENTRY;
	}

	/* And thats it :) 
	 * now just update the cr3 register :p
	 */

	reload_page_directory();

	/* RECURSIVE RANGE */
	rec_range.flags = VMM_REGION_USED | VMM_REGION_KERNEL;
	rec_range.next = 0;
	rec_range.prev = &k_range;
	rec_range.length = (X86_PAGE_SIZE * X86_PAGE_N_ENTRIES)*2;
	rec_range.start_address = X86_CURRENT_ADDR_SPACE - (X86_PAGE_SIZE * X86_PAGE_N_ENTRIES);

	/* KERNEL AVAILABLE RANGE */
	k_range.flags = VMM_REGION_USABLE;
	k_range.next = &rec_range;
	k_range.prev = &kactive_range;
	k_range.length = (uint32_t)(rec_range.start_address - (uint32_t)&kernel_end);
	k_range.start_address = (uint32_t)(&kernel_end);

	/* KERNEL ACTIVE RANGE */
	kactive_range.flags = VMM_REGION_USED | VMM_REGION_KERNEL;
	kactive_range.next = &k_range;
	kactive_range.prev = &io_range;
	kactive_range.length = ks;
	kactive_range.start_address = (X86_KERNEL_PAGE_IDX << X86_VIRT_ADDR_PDI_RSHFT);

	/* IO RANGE */
	io_range.flags = VMM_REGION_USABLE;
	io_range.next = &kactive_range;
	io_range.prev = &user_range;
	io_range.length = (X86_PAGE_SIZE * X86_PAGE_N_ENTRIES);
	io_range.start_address = (X86_IO_PAGE_IDX << X86_VIRT_ADDR_PDI_RSHFT);

	/* USER RANGE */
	user_range.flags = VMM_REGION_UNUSBALE | VMM_REGION_USER;
	user_range.next = &io_range;
	user_range.prev = 0;
	user_range.length = io_range.start_address;
	user_range.start_address = 0;

	out_context->arch = &arch_data_vmm;
	out_context->space = (struct vmm_addr_space*)rec_range.start_address;
	out_context->region_map = &user_range;
}

uint8_t
arch_vmm_create_context(struct vmm_context* ctx, struct vmm_context* res)
{
	/* we first allocate the bare minimum, a page directory */
	physical_address_t page_directory_phy = pmm_alloc_page();

	if (page_directory_phy == 0) {
		return 1;
	}
	
	struct vmm_addr_space* current = (struct vmm_addr_space*)X86_CURRENT_ADDR_SPACE;
	
	/* use the swap page in order to init the page with 0s and recursive last entry */
	current->p_dir.pdes[X86_SWAP_PAGE_IDX] = (page_directory_phy | X86_PAGE_FLAG_KERNEL);

	reload_page_directory();

	for (int i = 0; i < X86_IO_PAGE_IDX; i++) {
		current->p_tables[X86_SWAP_PAGE_IDX].ptes[i] = X86_PAGE_NULL_ENTRY;
	}

	for (int i = X86_IO_PAGE_IDX; i < X86_PAGE_N_ENTRIES - 1; i++) {
		current->p_tables[X86_SWAP_PAGE_IDX].ptes[i] = current->p_dir.pdes[i];
	}

	current->p_tables[X86_SWAP_PAGE_IDX].ptes[0x3ff] = (page_directory_phy | X86_PAGE_FLAG_KERNEL);

	reload_page_directory();
	
	/* 
	 * now the address space is ready, but we should map it somewehe in memory
	 * so that the user ( kernel actually ) can modify it before using it.
	 */

	struct vmm_arch_data* user_arch_data = kalloc(sizeof(struct vmm_arch_data));
	user_arch_data->cr3 = page_directory_phy;

	res->arch = user_arch_data;
	res->space = 0; /* space is set to 0 for now, not yet mapped */

	return 0;

}

void
arch_vmm_unmap(struct vmm_context* ctx, ptr_t p)
{
	uint32_t pdi = X86_ADDR_TO_PDI(p);
	uint32_t pti = X86_ADDR_TO_PTI(p);

	struct vmm_addr_space* addr_space = (struct vmm_addr_space*)X86_CURRENT_ADDR_SPACE;

	if (ctx->arch->cr3 != get_current_cr3()) {
		/* this means someone is trying to map into someone elses */
		/* temporal mapping */
		addr_space->p_dir.pdes[X86_SWAP_PAGE_IDX] = ctx->arch->cr3 | X86_PAGE_FLAG_KERNEL;

		addr_space = (struct vmm_addr_space*)(X86_SWAP_PAGE_IDX << X86_VIRT_ADDR_PDI_RSHFT);
	}

	if (!(addr_space->p_dir.pdes[pdi] & X86_PAGE_FLAG_PRESENT)) {
		// if not present what tha hell
		return;
	}

	addr_space->p_tables[pdi].ptes[pti] = X86_PAGE_NULL_ENTRY;

}

physical_address_t
arch_vmm_virt_to_phys(struct vmm_context* ctx, ptr_t p)
{
	uint32_t pdi = X86_ADDR_TO_PDI(p);
	uint32_t pti = X86_ADDR_TO_PTI(p);
	
	struct vmm_addr_space* addr_space = (struct vmm_addr_space*)X86_CURRENT_ADDR_SPACE;

	if (ctx->arch->cr3 != get_current_cr3()) {
		/* this means someone is trying to map into someone elses */
		/* temporal mapping */
		addr_space->p_dir.pdes[X86_SWAP_PAGE_IDX] = ctx->arch->cr3 | X86_PAGE_FLAG_KERNEL;

		addr_space = (struct vmm_addr_space*)(X86_SWAP_PAGE_IDX << X86_VIRT_ADDR_PDI_RSHFT);
	}

	if (!(addr_space->p_dir.pdes[pdi] & X86_PAGE_FLAG_PRESENT)) {
		// if not present what tha hell
		return 0;
	}

	if (!(addr_space->p_tables[pdi].ptes[pti] & X86_PAGE_FLAG_PRESENT)) {
		return 0;
	}

	return (addr_space->p_tables[pdi].ptes[pti] & X86_PDE_PT_MASK); 
	
}

static uint32_t
arch_flags_from_kernel_flags(uint32_t flags)
{
	uint32_t res = X86_PAGE_FLAG_PRESENT;
	if (flags & VMM_PROT_USER) {
		res |= X86_PAGE_FLAG_U_PERM;
	}

	if (flags & VMM_PROT_WRITE) {
		res |= X86_PAGE_FLAG_RW;
	}

	return res;
}

void
arch_vmm_map(struct vmm_context* vmm, ptr_t v, physical_address_t p, uint32_t n_pages, uint32_t flags)
{
	uint32_t pdi = X86_ADDR_TO_PDI(v);
	uint32_t pti = X86_ADDR_TO_PTI(v);

	struct vmm_addr_space* addr_space = (struct vmm_addr_space*)X86_CURRENT_ADDR_SPACE;

	uint32_t arch_flags = arch_flags_from_kernel_flags(flags);

	if (vmm->arch->cr3 != get_current_cr3()) {
		/* this means someone is trying to map into someone elses */
		/* temporal mapping */
		addr_space->p_dir.pdes[X86_SWAP_PAGE_IDX] = vmm->arch->cr3 | X86_PAGE_FLAG_KERNEL;
		reload_page_directory();

		addr_space = (struct vmm_addr_space*)(X86_SWAP_PAGE_IDX << X86_VIRT_ADDR_PDI_RSHFT);
	}

	if (!(addr_space->p_dir.pdes[pdi] & X86_PAGE_FLAG_PRESENT)) {
		// in this case we need a new page table!!
		physical_address_t n_pt = pmm_alloc_page();
		addr_space->p_dir.pdes[pdi] = (n_pt | X86_PAGE_FLAG_USER);
		reload_page_directory();
	}

	for (int i = 0; i < n_pages; i++) {
		addr_space->p_tables[pdi].ptes[pti+i] = ((p+(i*X86_PAGE_SIZE)) | arch_flags);
	}

	reload_page_directory();	
}
