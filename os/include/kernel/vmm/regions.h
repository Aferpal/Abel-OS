#ifndef _VMM_REGIONS_H_
#define _VMM_REGIONS_H_

#include "kernel/vmm/types.h"

typedef enum vmm_region_status {
    VMM_REGION_SUCCESS,
    VMM_REGION_FAIL,
    VMM_REGION_BAD_ARGS
} vmm_region_status_e;

void vmm_region_pool_init(void);

struct vmm_region* get_region_from_ptr(struct vmm_region* region_list, ptr_t ptr);

struct vmm_region* vmm_find_free_range(struct vmm_region* region_list, uint32_t len);

uint32_t vmm_reserve_range(struct vmm_region* region, ptr_t start, uint32_t len, uint32_t flags);

uint32_t vmm_reserve_io_range(struct vmm_region* region, ptr_t start, uint32_t len, uint32_t flags);

uint32_t vmm_free_range(struct vmm_region* region_list, ptr_t start, uint32_t len, uint32_t flags);

uint32_t vmm_region_init_region_map(struct vmm_region**);

#endif