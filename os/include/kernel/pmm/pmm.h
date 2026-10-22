#ifndef _PMM_H_
#define _PMM_H_

#include <stdint.h>
#include <kernel/boot/types.h>

uint8_t pmm_init(struct boot_info*);

physical_address_t pmm_alloc_page();

uint32_t pmm_get_page(physical_address_t);

void pmm_free_page_n(uint32_t);

void pmm_free_page(physical_address_t);

void pmm_remap_self_data();

physical_address_t pmm_alloc_pages(uint32_t);

#endif
