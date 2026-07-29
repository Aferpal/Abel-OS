#include "arch/x86/interrupt/pic.h"
#include "arch/x86/io.h"

void
pic_send_eoi(uint8_t irqn)
{
	if (irqn >= 8) {
		outb(PIC2_COMMAND, PIC_CMD_EOI);
	}

	outb(PIC1_COMMAND, PIC_CMD_EOI);
}

void
pic_remap()
{
    // init sequence
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

	// indicate offset
	outb(PIC1_DATA, 0x20);
	outb(PIC2_DATA, 0x28);

	// indicate master-slave
	outb(PIC1_DATA, ICW3_MASTER_SLAVE2);
	outb(PIC2_DATA, ICW3_SLAVE_ID2);

	// Set 8086 
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
}

void
pic_mask_all()
{
    outb(0x21, 0xff);
	outb(0xa1, 0xff);
}

void
pic_unmask_all()
{
	outb(0x21, 0x00);
	outb(0xa1, 0x00);
}

void pic_set_mask(uint8_t);