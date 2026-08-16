#ifndef _PMM_HAL_H_
#define _PMM_HAL_H_

#include <kernel/pmm/types.h>

void arch_pmm_init(struct boot_info*, struct physical_memory_map*);

#endif
