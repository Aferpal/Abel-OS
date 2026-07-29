#ifndef _X86_PIT_H_
#define _X86_PIT_H_

#include "stdint.h"

#define PIT_CHANNEL0    0x40
#define PIT_CHANNEL1    0x41
#define PIT_CHANNEL2    0x42

#define PIT_COMMAND     0x43

#define PIT_SC_CHANNEL0 0x00
#define PIT_SC_CHANNEL1 0x40
#define PIT_SC_CHANNEL2 0x80
#define PIT_SC_CHANNELRB 0xc0

#define PIT_RW_LATCH 0x00
#define PIT_RW_LSB 0x10
#define PIT_RW_MSB 0x20
#define PIT_RW_LMSB 0x30

#define PIT_MODE_ONESHOT 0x00
#define PIT_MODE_RATE 0x04
#define PIT_MODE_SQWAV 0X06

#define PIT_BCD 0x01
#define PIT_BIN 0x00

#define PIT_BASE_FREQ_HZ 1193182

#define DEFAULT_PIT_INTERRUPT_FREQ_HZ 100

void pit_init();

void pit_set_frequency(uint16_t);

uint64_t pit_ticks(void);

uint16_t pit_frequency(void);

#endif