#ifndef _VMM_HAL_H_
#define _VMM_HAL_H_

#include "kernel/vmm/types.h"

struct vmm_addr_space* arch_vmm_create_address_space();

void arch_vmm_map(struct vmm_addr_space* ad_space, physical_address_t phys, virtual_address_t virt);

void arch_vmm_unmap(struct vmm_addr_space* ad_space, virtual_address_t virt);



#endif
