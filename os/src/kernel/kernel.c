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
#include "kernel/scheduler/scheduler.h"
#include "kernel/scheduler/proc.h"
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
main(void)
{

	uint64_t last_tick = 0;
	uint64_t time = 0;
	uint16_t pit_freq = pit_frequency();	
	while (1) {
		printk("Task 1 is running\n");
		while (1) {
			time = pit_ticks();
		
			if (time > (last_tick + 100)) {
				last_tick = time;
				break;
			}
		}
	}

	return;
}

void
task2(void)
{

        uint64_t last_tick = 0;
	uint64_t time = 0;
	uint16_t pit_freq = pit_frequency();
        while (1) {
                printk("Task 2 is running\n");
                while (1) {
                        time = pit_ticks();

                        if (time > (last_tick + pit_freq)) {
                                last_tick = time;
                                break;
                        }
                }
        }

        return;
}

void
idle(void)
{
	printk("Idle task\n");
	for(;;) {
	
	}
}

void
kernel_main(struct boot_info* b)
{
	clear_screen();
	printk("Abel-OS\n\n\t");
	printk("Welcome to my operating system!\n\n");

	pmm_init(b);

	vmm_init(b);

	init_int_subsystem();

	input_subsystem_init();

	pit_init();

	ps2_keyboard_init();

	arch_enable_interrupts();

	scheduler_init(0);
	
	uint32_t stack_init = proc_create_process(main);

	scheduler_add_process(stack_init, 0);

	uint32_t stack_task2 = proc_create_process(task2);

	scheduler_add_process(stack_task2, 0);

	uint32_t stack_idle = proc_create_process(idle);

	scheduler_add_process(stack_idle, 0);

	scheduler_start();

	printk("=================================================\n");
	printk("***************** ERROR *************************\n");
	printk("=================================================\n\n");
	printk("Something very unexpected happened and this code\n");
	printk("should never be reached, scheduler did not switch\n");
	printk("to a first process after starting and returned\n\n\n\n");

	for (;;) {

	}

}
