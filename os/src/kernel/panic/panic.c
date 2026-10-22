#include "kernel/panic/panic.h"
#include "kernel/printk.h"

void
panic(const char* what, uint32_t len) 
{
    printk("[ KERNEL_PANIC ] An unfixable error ocurred: %s\n", what);
    printk("HALTING...");

    while (1) {

    }
}