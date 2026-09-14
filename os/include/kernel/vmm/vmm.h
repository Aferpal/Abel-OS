#ifndef _VMM_H_
#define _VMM_H_

#include "kernel/boot/types.h"
#include "kernel/vmm/types.h"

void vmm_init(struct boot_info*);

void vmm_map(struct vmm_addr_space*, ptr_t, ptr_t, uint32_t);

void vmm_unmap(struct vmm_addr_space*, ptr_t);

void vmm_change_to_addr_space(struct vmm_addr_space*);

#endif

