#include "kernel/printk.h"
#include "drivers/keyboard.h"
#include "kernel/interrupt/interrupts.h"
#include "arch/interrupt_hal.h"
#include "kernel/printk.h"
#include "arch/x86/timer/pit.h"
#include "drivers/keyboard.h"
#include "kernel/input/input.h"
#include "kernel/pmm/pmm.h"
#include "kernel/kalloc/kalloc.h"
#include "stdint.h"

void
kalloc_test()
{
	printk("Running kalloc test\n");
	printk("=============Test 1==============\n");
	void* p1 = kalloc(1024);
	void* p2 = kalloc(1023);
	void* p3 = kalloc(1024);
	void* p4 = kalloc(1024);
	void* p5 = kalloc(128);
	void* p6 = kalloc(128);
	printk("P1: %x, P2: %x, P3: %x, P4: %x, P5: %x, P6: %x\n", p1, p2, p3, p4, p5, p6);
	kfree(p4);
	printk("==============Test 2==============\n");
	p4 = kalloc(1024);
	kfree(p3);
	kfree(p2);
	kfree(p5);
	kfree(p6);
	printk("==============Test 3==============\n");
	kfree(p1);
	p1 = kalloc(4088);
	printk("P1: %x, P4: %x\n", p1, p4);
	kfree(p4);
	kfree(p1);
	printk("==============Test 4==============\n");
	p1 = kalloc(4075);
	p2 = kalloc(5);
	printk("P1: %x, P2: %x\n", p1, p2);

}

void
kernel_main(struct boot_info* b)
{
	clear_screen();
	printk("Abel-OS\n\n\t");
	printk("Welcome to my operating system!\n\n");

	pmm_init(b);

	init_int_subsystem();

	input_subsystem_init();

	pit_init();

	ps2_keyboard_init();

	arch_enable_interrupts();

	struct input_event event;

	printk("\n\n\n\n");

	kalloc_test();

	while (1) {
		if (!input_event_pop(&event)) {
			continue;
		}

		if (event.key_evt.type == KEY_PRESS) {
			printk("Press: ");
		} else {
			printk("Release: ");
		}
		printk("%s\n", get_key_name(event.key_evt.value));
	}

}
