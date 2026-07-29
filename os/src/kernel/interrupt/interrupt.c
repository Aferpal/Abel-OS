#include "kernel/interrupt/interrupts.h"
#include "kernel/interrupt/irq.h"
#include "arch/interrupt_hal.h"
#include "arch/irq_hal.h"
#include "arch/exception_hal.h"

void
interrupt_dispatch(struct interrupt_frame *frame)
{

	if (arch_vector_is_exception(frame)) {

        arch_exception_dispatch(frame);

	} else if (arch_vector_is_irq(frame)) {

		irq_dispatch(frame);

	}
}

void
init_int_subsystem()
{

    arch_init_int_subsytem();

    
}