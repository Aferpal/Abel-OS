#ifndef _VMM_H_
#define _VMM_H_

#include "kernel/boot/types.h"
#include "kernel/vmm/defines.h"
#include "kernel/vmm/types.h"

void vmm_init(struct boot_info*);

uint8_t vmm_create_context(struct vmm_context*);

ptr_t vmm_map_core(physical_address_t base, uint32_t len);

void vmm_map(struct vmm_context*, ptr_t, physical_address_t, uint32_t flags);

void vmm_unmap(struct vmm_context*, ptr_t);

ptr_t vmm_alloc_page(uint32_t flags);

void vmm_free_page(ptr_t);

uint32_t vmm_get_page(ptr_t);

#endif

