#ifndef _IDT_H_
#define _IDT_H_

struct idt_entry {
	unsigned short offset_low;
	unsigned short segment;
	unsigned char reserved;
	unsigned char flags;
	unsigned short offset_high;
}__attribute__((packed));


struct idt_descriptor {
	unsigned short size;	
	unsigned int offset;
}__attribute__((packed));

void idt_init();
void idt_set_entry(unsigned char, unsigned int, unsigned short, unsigned char);
void idt_load(struct idt_descriptor*);
void sti();

#endif
