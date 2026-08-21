#ifndef _BOOT_INFO_H_
#define _BOOT_INFO_H_

#include <arch/x86/boot/e820.h>

struct boot_info {
	uint32_t magic;
	struct e820_frame_info* mmap_list_ptr;
	uint16_t n_entries;
	uint16_t padding;
	
	physical_address_t bootstrap_start;
	physical_address_t bootstrap_end;
};



#endif
