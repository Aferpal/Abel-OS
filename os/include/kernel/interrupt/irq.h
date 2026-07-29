#ifndef _IRQ_H_
#define _IRQ_H_

#include "kernel/interrupt/types.h"

#define MAX_IRQ_NUMBER 16

typedef enum irq_vector irq_vector_e;

enum irq_vector{
    IRQ_PIT,
    IRQ_KEYBOARD
};

void register_irq_handler(irq_vector_e, interrupt_handler_t);

void unregister_irq_handler(irq_vector_e);

void irq_dispatch(struct interrupt_frame*);

#endif