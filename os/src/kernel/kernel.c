#include "kernel/printk.h"
#include "drivers/keyboard.h"
#include "kernel/interrupt/interrupts.h"
#include "arch/interrupt_hal.h"
#include "kernel/printk.h"
#include "arch/x86/timer/pit.h"
#include "drivers/keyboard.h"
#include "kernel/input/input.h"

void
kernel_main(void)
{
	clear_screen();
	printk("Abel-OS\n\n\t");
	printk("Welcome to my operating system!\n\n");
	
	init_int_subsystem();

	input_subsystem_init();

	pit_init();

	ps2_keyboard_init();

	arch_enable_interrupts();

	struct input_event event;
	
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
