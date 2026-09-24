#include "arch/arch_hal.h"
#include "kernel/pmm/pmm.h"
#include "kernel/vmm/vmm.h"

void
arch_early_setup(struct boot_info* b)
{
	pmm_init(b);
	vmm_init(b);
	pmm_remap_self_data();
}
