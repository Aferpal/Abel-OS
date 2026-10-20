#ifndef _VMM_HAL_H_
#define _VMM_HAL_H_

#include "stdint.h"
#include "kernel/vmm/types.h"
#include "kernel/boot/types.h"

void arch_vmm_init(struct boot_info*, struct vmm_context*);

uint8_t arch_vmm_create_context(struct vmm_context*);

ptr_t arch_vmm_map_core(physical_address_t base, uint32_t len);

void arch_vmm_map(struct vmm_context*, ptr_t, physical_address_t, uint32_t flags);

ptr_t arch_vmm_find_unused_page_kernel();

void arch_vmm_unmap(ptr_t);

physical_address_t arch_vmm_virt_to_phys(ptr_t);

#endif
