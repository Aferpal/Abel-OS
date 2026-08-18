#include "kernel/interrupt/interrupts.h"
#include "kernel/printk.h"
#include "arch/x86/interrupt/idt.h"
#include "arch/x86/interrupt/pic.h"
#include "arch/interrupt_hal.h"

void
arch_enable_interrupts()
{
	pic_unmask_all();
}

void
arch_disable_interrupts()
{
	pic_mask_all();
}

void
arch_init_int_subsytem()
{
    printk(" * INIT IDT\n");

	idt_init();
	
	printk(" * IDT TABLE INITIALIZED\n");

	printk(" * REMAP IRQs\n");

	pic_remap();

	printk(" * MASK ALL IRQs\n");

	pic_mask_all();

	printk(" * STI\n");

	sti();

}
