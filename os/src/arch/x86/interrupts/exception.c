#include "arch/exception_hal.h"
#include "kernel/printk.h"

#define EXCEPTION_START 0
#define EXCEPTION_END 31
#define N_EXCEPTIONS (EXCEPTION_END - EXCEPTION_START)

void 
zero_division_handler(struct interrupt_frame* frame)
{
    printk("DIVISION BY ZERO WAS ATTEMPTED...\n");
    for (;;) {

    }
}


static interrupt_handler_t exception_handlers[N_EXCEPTIONS] = {
    zero_division_handler
};

int
arch_vector_is_exception(struct interrupt_frame* frame)
{
    return frame->int_number >= EXCEPTION_START && frame->int_number <= EXCEPTION_END;
}

void
arch_exception_dispatch(struct interrupt_frame* frame)
{
    printk("--------------------------------\n");
    printk("EXCEPTION %d\n", frame->int_number);
	printk("Regdump: \n");
	printk("  EAX: %x\n  EBX: %x\n  ECX: %x\n  EDX: %x\n", frame->eax, frame->ebx, frame->ecx, frame->edx);
	printk("  ESP: %x\n  EDI: %x\n  ESI: %x\n  EIP: %x\n", frame->esp, frame->edi, frame->esi, frame->eip);
	printk("\n--------------------------------\n");

    if (exception_handlers[frame->int_number - EXCEPTION_START]) {
        exception_handlers[frame->int_number - EXCEPTION_START](frame);
    }
    for (;;) {

    }
}