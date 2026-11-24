#include "arch/x86/interrupt/idt.h"
#include "arch/x86/syscall/frame.h"
#include "arch/x86/syscall/defines.h"
#include "arch/syscall_hal.h"

extern void syscall_stub();

syscall_dispatcher_t syscall_dispatch;

void
arch_syscall_init(syscall_dispatcher_t i_syscall_dispatch)
{
    idt_set_entry(SYSCALL_INTERRUPT_VECTOR, (uint32_t)syscall_stub, 0x08, 0xEF);
    syscall_dispatch = i_syscall_dispatch;
}

void
arch_syscall_dispatch(struct syscall_frame *frame)
{
	frame->eax = syscall_dispatch(
        frame->eax,
        frame->ebx,
        frame->ecx,
        frame->edx,
        frame->esi,
        frame->edi
    );
}