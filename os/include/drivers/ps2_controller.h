#ifndef _PS2_CONTROLLER_H_
#define _PS2_CONTROLLER_H_

#include "stdint.h"

#define PS2_DATA_PORT 0x60

#define PS2_STATUS_PORT 0x64

#define PS2_CMD_PORT 0x64

#define PS2_OUT_BUFFER_FULL 0x01

#define PS2_IN_BUFFER_FULL 0x02

uint8_t ps2_read_data();

uint8_t ps2_read_status();

void ps2_send_command(uint8_t);

#endif