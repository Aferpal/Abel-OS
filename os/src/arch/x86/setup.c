#include "arch/arch_hal.h"
#include "kernel/pmm/pmm.h"
#include "kernel/vmm/vmm.h"
#include "kernel/panic/panic.h"

void
arch_early_setup(struct boot_info* b)
{
	if (pmm_init(b) != 0) {
		// panic?
		panic("Error trying to init pmm\n", 0);
	}
	vmm_init(b);
	pmm_remap_self_data();
}
