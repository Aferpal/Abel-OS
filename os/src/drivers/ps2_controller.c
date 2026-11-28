#include "drivers/ps2_controller.h"
#include "arch/io_hal.h"

uint8_t
ps2_read_data()
{
    return arch_port_read8(PS2_DATA_PORT);
}

uint8_t
ps2_read_status()
{
    return arch_port_read8(PS2_STATUS_PORT);
}

void
ps2_send_command(uint8_t cmd)
{
    arch_port_write8(PS2_CMD_PORT, cmd);
}