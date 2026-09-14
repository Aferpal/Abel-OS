#include "arch/vmm_hal.h"
#include "arch/x86/vmm/types.h"

physical_address_t k_page_dir_addr;

physical_address_t k_swap_page_table_addr;

physical_address_t k_io_page_table_addr;

physical_address_t k_code_page_table_addr;



void
arch_vmm_init(struct boot_info* b)
{
	// :)
	
}
