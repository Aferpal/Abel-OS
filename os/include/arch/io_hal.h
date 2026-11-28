#ifndef _ARCH_IO_H_
#define _ARCH_IO_H_

#include "stdint.h"

/* PORT IO */
/*read*/
uint8_t arch_port_read8(uint16_t port);
uint16_t arch_port_read16(uint16_t port);
uint32_t arch_port_read32(uint16_t port);

/*write*/
void arch_port_write8(uint16_t port, uint8_t value);
void arch_port_write16(uint16_t port, uint16_t value);
void arch_port_write32(uint16_t port, uint32_t value);


/* MMIO */
/*read*/
uint8_t arch_mmio_read8(uint32_t addr);
uint16_t arch_mmio_read16(uint32_t addr);
uint32_t arch_mmio_read32(uint32_t addr);

/*write*/
void arch_mmio_write8(uint32_t addr, uint8_t value);
void arch_mmio_write16(uint32_t addr, uint16_t value);
void arch_mmio_write32(uint32_t addr, uint32_t value);

#endif