#include "arch/x86/timer/pit.h"
#include "kernel/interrupt/types.h"
#include "kernel/interrupt/irq.h"
#include "arch/x86/io.h"
#include "stdint.h"

uint64_t pit_ticks_from_system_start = 0;

static uint16_t current_interrupt_freq_hz = DEFAULT_PIT_INTERRUPT_FREQ_HZ; 

static void
pit_irq_handler(struct interrupt_frame* frame)
{
    pit_ticks_from_system_start++;
}

static void
config_pit_freq(uint16_t freq)
{
    uint16_t divisor = PIT_BASE_FREQ_HZ / freq;

    outb(PIT_CHANNEL0, divisor & 0xff);

    outb(PIT_CHANNEL0, divisor >> 8);
}

void
pit_init()
{
    outb(PIT_COMMAND, PIT_SC_CHANNEL0 | PIT_RW_LMSB | PIT_MODE_RATE | PIT_BIN);

    config_pit_freq(current_interrupt_freq_hz);

    register_irq_handler(IRQ_PIT, pit_irq_handler);
}

void
pit_set_frequency(uint16_t new_freq)
{
    current_interrupt_freq_hz = new_freq;
    
    config_pit_freq(new_freq);
}

uint64_t
pit_ticks(void)
{
    return pit_ticks_from_system_start;
}

uint16_t 
pit_frequency(void)
{
    return current_interrupt_freq_hz;
}