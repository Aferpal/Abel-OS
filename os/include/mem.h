#ifndef _STRING_H_
#define _STRING_H_

#include "stdint.h"

void memcpy(void* src, void* dst, uint32_t len); 

void memset(uint8_t val, void *dst, uint32_t len);

#endif
