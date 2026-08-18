#include "arch/x86/interrupt/idt.h"
#include "arch/x86/interrupt/interrupt_stubs.h"
#include "kernel/printk.h"

static struct idt_entry idt[256];

static struct idt_descriptor idtr;



void (*isr_stubs[32])() =
{
    isr0,
    isr1,
    isr2,
    isr3,
    isr4,
    isr5,
    isr6,
    isr7,
    isr8,
    isr9,
    isr10,
    isr11,
    isr12,
    isr13,
    isr14,
    isr15,
    isr16,
    isr17,
    isr18,
    isr19,
    isr20,
    isr21,
    isr22,
    isr23,
    isr24,
    isr25,
    isr26,
    isr27,
    isr28,
    isr29,
    isr30,
    isr31
};

void (*irq_stubs[16])() =
{
    irq0,
    irq1,
    irq2,
    irq3,
    irq4,
    irq5,
    irq6,
    irq7,
    irq8,
    irq9,
    irq10,
    irq11,
    irq12,
    irq13,
    irq14,
    irq15
};

void
idt_init()
{
	printk("[ IDT_INIT ] Begin\n");

	// init idtr
	idtr.offset = (unsigned int)&idt[0];
	idtr.size= (unsigned short)(sizeof(idt) - 1);

	printk("[ IDT_INIT ] Descriptor ready\n");
	// init idt
	for (int i=0; i < 32; i++) {
		idt_set_entry(i, (unsigned int)isr_stubs[i], 0x08, 0x8E);
	}

	printk("[ IDT_INIT ] Intel reserved 32 entries ready\n");

	for (int i = 0; i < 16; i++) {
		idt_set_entry(i+32, (unsigned int)irq_stubs[i], 0x08, 0x8E);
	}

    for (int i = 32+16; i < 255; i++) {
        idt_set_entry(i, (unsigned int)default_isr, 0x08, 0x8E);
    }

	printk("[ IDT_INIT ] All entries ready\n");

	idt_load(&idtr);

}

void
idt_set_entry(unsigned char idx, unsigned int offset, unsigned short segment, unsigned char flags)
{
	idt[idx].offset_low = offset & 0x0ffff;
	idt[idx].offset_high = offset >> 16;
	idt[idx].segment = segment;
	idt[idx].reserved = 0;
	idt[idx].flags = flags;
}
