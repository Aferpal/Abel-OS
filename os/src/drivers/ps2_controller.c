#include "drivers/ps2_controller.h"
#include "arch/x86/io.h"

uint8_t
ps2_read_data()
{
    return inb(PS2_DATA_PORT);
}

uint8_t
ps2_read_status()
{
    return inb(PS2_STATUS_PORT);
}

void
ps2_send_command(uint8_t cmd)
{
    outb(PS2_CMD_PORT, cmd);
}