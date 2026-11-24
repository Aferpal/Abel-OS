#include "arch/syscall_hal.h"
#include "kernel/syscall/syscall.h"
#include "kernel/syscall/defines.h"

/* used for syscall handlers */
#include "kernel/printk.h"
#include "kernel/scheduler/scheduler.h"

#define MAX_SYSCALLS 32

SYSCALL_DEFINE(hello_world)
{
    printk("Hello world from user space!\n");
    return 0;
} 

SYSCALL_DEFINE(yield)
{
    printk("Yielding...\n");
    scheduler_schedule();
    return 0;
}

static const syscall_handler_t
syscall_table[MAX_SYSCALLS] = {
    SYSCALL_FROM_NAME(hello_world),
    SYSCALL_FROM_NAME(yield)
};

static int32_t
syscall_dispatch(uint32_t syscall_number, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    if (syscall_number < 0 || syscall_number >= MAX_SYSCALLS || syscall_table[syscall_number] == 0) {
        return -ENOTIM;
    }
    
    return syscall_table[syscall_number](a1, a2, a3, a4, a5);
}

void
init_syscall_subsystem()
{
    arch_syscall_init(syscall_dispatch);
}