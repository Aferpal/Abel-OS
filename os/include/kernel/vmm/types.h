#ifndef _VMM_TYPES_H_
#define _VMM_TYPES_H_

#include "stdint.h"

typedef struct vmm_arch_data vmm_arch_data_s;
typedef struct vmm_addr_space vmm_addr_space_s;

struct vmm_region {
    ptr_t start_address;
    uint32_t length;
    uint32_t flags;
    struct vmm_region* next;
    struct vmm_region* prev;
};

struct vmm_context {
    struct vmm_region* region_map;
    struct vmm_arch_data* arch;
    struct vmm_addr_space* space;
};

#endif
