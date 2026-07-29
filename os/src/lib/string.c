#include "string.h"


void
memcpy(void* src, void* dst, uint32_t len)
{
	uint8_t *src_c = (uint8_t*) src;
	uint8_t *dst_c = (uint8_t*) dst;
	uint32_t i = 0;
	while (i < len) {
		dst_c[i] = src_c[i];
		i++;
	}
}

uint32_t
strlen(char *str)
{
	if (str == (void*)0) {
		return -1;
	}
	uint32_t res = 0;
	while (str[res] != 0) {
		res++;
	}
	return res;
}
