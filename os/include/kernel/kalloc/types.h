#ifndef _KALLOC_TYPES_H_
#define _KALLOC_TYPES_H_

#include "stdint.h"

enum kobject_header_props {
	KOBJECT_HEADER_FREE = 0x0,
	KOBJECT_HEADER_USED = 0x01
};

struct kobject_header {
	uint32_t size;
	union {
		uint32_t next;
		uint32_t props;
	};
};

#endif
