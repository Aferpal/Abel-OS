#ifndef _IRQ_HAL_H_
#define _IRQ_HAL_H_

#include "kernel/interrupt/irq.h"

int arch_vector_is_irq(struct interrupt_frame* frame);

irq_vector_e arch_vector_to_irqn(struct interrupt_frame*);

void arch_end_irq(irq_vector_e);

#endif