#ifndef _BOOT_E820_H_
#define _BOOT_E820_H_

#include <stdint.h>

enum E820_TYPE {
	MEM_USABLE = 1,
	MEM_RESERVED = 2,
	MEM_ACPI_REUSABLE = 3,
	MEM_ACPI = 4,
	MEM_CORRUPTED = 5
};

struct e820_frame_info {
	uint64_t base;
	uint64_t length;
	uint32_t type;
	uint32_t ext;
};
 

#endif
