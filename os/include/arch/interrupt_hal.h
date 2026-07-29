#ifndef _INTERRUPT_HAL_H_
#define _INTERRUPT_HAL_H_

#include "kernel/interrupt/types.h"

void arch_init_int_subsytem();

void arch_enable_interrupts();

void arch_disable_interrupts();

#endif