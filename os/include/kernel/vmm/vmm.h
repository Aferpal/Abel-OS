#ifndef _VMM_H_
#define _VMM_H_

#include "kernel/vmm/types.h"

void vmm_init();

/* creates an address space, that is in x86 a page directory */
struct vmm_addr_space* vmm_create_address_space();

/* maps a certain physical address into a virtual address in the addr space given */
void vmm_map(struct vmm_addr_space*, ptr_t, ptr_t); 

/* clears the virtual address in an address space */
void vmm_unmap(struct vmm_addr_space*, ptr_t);


#endif
