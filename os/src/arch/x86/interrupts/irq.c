#include "arch/irq_hal.h"
#include "arch/x86/interrupt/pic.h"

#define IRQ_START 32
#define IRQ_END 48

int
arch_vector_is_irq(struct interrupt_frame* frame)
{
	return frame->int_number >= IRQ_START && frame->int_number <= IRQ_END;
}

irq_vector_e
arch_vector_to_irqn(struct interrupt_frame* frame)
{
    return frame->int_number - IRQ_START;
}

void
arch_end_irq(irq_vector_e irqn)
{
    pic_send_eoi(irqn);
}