#include "kernel/scheduler/scheduler.h"
#include "kernel/scheduler/fifo.h"
#include "kernel/pmm/pmm.h"
#include "kernel/printk.h"
#include "kernel/interrupt/irq.h"

extern void arch_switch_context(struct pcb*, struct pcb*);

static struct sched_ops impl;

static struct pcb* current;

static struct pcb exit_helper;

static pid_t last_pid;


static void
irq_handler(struct interrupt_frame* frame) 
{
	scheduler_tick();
}

void
scheduler_init(struct sched_ops* ops)
{

	if (ops == 0) {
		impl.init = sched_fifo_init;
		impl.enqueue_proc = sched_fifo_enqueue;
		impl.dequeue_proc = sched_fifo_dequeue;
		impl.get_next_proc = sched_fifo_next;
		impl.tick = sched_fifo_tick;
	} else {
		impl = *ops;
	}

	current = 0;
	last_pid = 0;

}

void
scheduler_add_process(uint32_t stack, uint32_t priority)
{
	struct pcb p = { .id = last_pid++, .priority = priority, .stack_ptr = stack, .status = READY, .runtime = 0 };
	
	if (impl.enqueue_proc) {
		impl.enqueue_proc(&p);
	}
}

void
scheduler_kill_process(pid_t pid)
{
	if (impl.dequeue_proc) {
		impl.dequeue_proc(pid);
	}
}

void
scheduler_tick()
{
	uint8_t must_reschedule = 0;

	current->runtime++;
	
	if (impl.tick) {
		must_reschedule = impl.tick(current);
	}

	if (must_reschedule != 0) {
		scheduler_schedule();
	}
}

void
scheduler_schedule()
{

	struct pcb* from = current;
	current = impl.get_next_proc(from);

	// is this process ending? then free its resources
	// the problem is something may be pushed into a 
	// now cleared stack, use a helper stack? or forget
	// since it is thrash
	if (from->flags == PROC_EXIT) {
		pmm_free_page((physical_address_t)from->stack_ptr);
		impl.dequeue_proc(from->id);
		from = &exit_helper;
	}
	
	if (from != current) {
		from->runtime = 0;
		arch_switch_context(from, current);
	}

}

void
scheduler_start()
{
	current = impl.get_next_proc(0);

	if (current == 0) {
		return;
	}

	// perform special jump to first process
	
	// maybe try to free our mem page?
	register_irq_post_routine(IRQ_PIT, irq_handler);
	
	arch_switch_context(&exit_helper, current);

	// if we get here something very very bad was going on
}


void
scheduler_exit_current()
{
	if (current == 0) {
		// panic?
		return;
	}

	current->flags = PROC_EXIT;
	
	scheduler_schedule();

}
