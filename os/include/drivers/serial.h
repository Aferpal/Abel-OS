#ifndef _SERIAL_H_
#define _SERIAL_H_
#include "stdint.h"
#define PORT 0x3f8

void init_rs232();

uint8_t serial_getc();

void serial_putc(uint8_t c);


#endif 