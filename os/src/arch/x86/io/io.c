#include "arch/io_hal.h"
#include "arch/x86/io.h"

/* PORT IO */
/*read*/
uint8_t
arch_port_read8(uint16_t port)
{
    return inb(port);
}

uint16_t
arch_port_read16(uint16_t port)
{
    return inw(port);
}

uint32_t
arch_port_read32(uint16_t port)
{
    return indw(port);
}

/*write*/
void
arch_port_write8(uint16_t port, uint8_t value)
{
    outb(port, value);
}

void
arch_port_write16(uint16_t port, uint16_t value)
{
    outw(port, value);
}

void
arch_port_write32(uint16_t port, uint32_t value)
{
    outdw(port, value);
}


/* MMIO */
/*read*/
uint8_t
arch_mmio_read8(uint32_t addr)
{
    return *(volatile uint8_t*)addr;
}

uint16_t
arch_mmio_read16(uint32_t addr)
{
    return *(volatile uint16_t*)addr;
}

uint32_t arch_mmio_read32(uint32_t addr)
{
    return *(volatile uint32_t*)addr;
}

/*write*/
void
arch_mmio_write8(uint32_t addr, uint8_t value)
{
    *(volatile uint8_t*)addr = value;
}

void arch_mmio_write16(uint32_t addr, uint16_t value)
{
    *(volatile uint16_t*)addr = value;
}

void arch_mmio_write32(uint32_t addr, uint32_t value)
{
    *(volatile uint32_t*)addr = value;
}
