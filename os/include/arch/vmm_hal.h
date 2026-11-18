#ifndef _VMM_HAL_H_
#define _VMM_HAL_H_

#include "stdint.h"
#include "kernel/vmm/types.h"
#include "kernel/boot/types.h"

void arch_vmm_init(struct boot_info*, struct vmm_context*);

uint8_t arch_vmm_create_context(struct vmm_context* current, struct vmm_context* res_context);

void arch_vmm_map(struct vmm_context*, ptr_t, physical_address_t, uint32_t n_pages, uint32_t flags);

void arch_vmm_unmap(struct vmm_context*, ptr_t);

physical_address_t arch_vmm_virt_to_phys(struct vmm_context*, ptr_t);

#endif
