#ifndef _VMM_H_
#define _VMM_H_

#include "kernel/boot/types.h"
#include "kernel/vmm/types.h"

void vmm_init(struct boot_info*);

ptr_t vmm_map_core(physical_address_t base, uint32_t len);

void vmm_map(struct vmm_addr_space*, ptr_t, physical_address_t);

void vmm_unmap(struct vmm_addr_space*, ptr_t);

ptr_t vmm_alloc_page();

void vmm_free_page(ptr_t);

uint32_t vmm_get_page(ptr_t);

void vmm_change_to_addr_space(struct vmm_addr_space*);

#endif

