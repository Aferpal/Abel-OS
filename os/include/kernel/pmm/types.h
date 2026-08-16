#ifndef _PMM_TYPES_H_
#define _PMM_TYPES_H_

#include <stdint.h>

#include <kernel/boot/types.h>

typedef struct physical_memory_map physical_memory_map_t;

struct physical_memory_map {
	uint8_t *bitmap;
	uint32_t bitmap_len;
	uint32_t n_pages;
};

#endif
