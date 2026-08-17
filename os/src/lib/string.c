#include "string.h"

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
