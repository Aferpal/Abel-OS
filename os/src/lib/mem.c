#include <mem.h>

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

void
memset(uint8_t val, void *dst, uint32_t len)
{
	uint8_t *dst_c = (uint8_t*) dst;
	uint32_t i = 0;
	while (i < len) {
		dst_c[i] = val;
		i++;
	}
}