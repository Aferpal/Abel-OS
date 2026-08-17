#ifndef _KALLOC_H_
#define _KALLOC_H_

#include "stdint.h"
#include "kernel/kalloc/types.h"

void*
kalloc(uint32_t);

void
kfree(void*);

#endif
