#include "kernel/interrupt/irq.h"
#include "arch/irq_hal.h"

static interrupt_handler_t irq_handlers[MAX_IRQ_NUMBER];

void
register_irq_handler(irq_vector_e irqn, interrupt_handler_t handler)
{
    irq_handlers[irqn] = handler;
}

void
unregister_irq_handler(irq_vector_e irqn)
{
    irq_handlers[irqn] = 0;
}

void
irq_dispatch(struct interrupt_frame* frame)
{
    irq_vector_e irqn = arch_vector_to_irqn(frame);
    
    if (irq_handlers[irqn]){
        irq_handlers[irqn](frame);
    }
    arch_end_irq(irqn);
}