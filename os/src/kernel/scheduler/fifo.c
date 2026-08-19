#include "kernel/scheduler/fifo.h"
#include "kernel/kalloc/kalloc.h"

#define MAX_PROC_QUANTUM 300

struct pcb_list {
	struct pcb* proc;
	struct pcb_list* next;
};

static struct pcb_list* proc_list;

static struct pcb_list* 
get_by_pid(pid_t pid)
{
	struct pcb_list* p = proc_list;
	
	while (p != 0 && p->proc->id != pid) {
		p = p->next;
	}

	return p;
}

uint8_t
sched_fifo_init(void)
{
	proc_list = 0;
}

uint8_t
sched_fifo_enqueue(struct pcb* p)
{

	if (p == 0) {
		return 1;
	}

	struct pcb* copy = kalloc(sizeof(struct pcb));

	if (copy == 0) {
		return 1;
	}

	*copy = *p;

	struct pcb_list* node = kalloc(sizeof(struct pcb_list));
	
	if (node == 0) {
		return 1;
	}

	node->proc = copy;
	node->next = 0;

	if (proc_list == 0) {
		proc_list = node;
	} else {
		node->next = proc_list->next;
		proc_list->next = node;
	}

	return 0;
}

uint8_t
sched_fifo_dequeue(pid_t pid)
{
	
	struct pcb_list** current = &proc_list;
	while ((*current) != 0 && (*current)->proc->id != pid) {
		current = &((*current)->next);
	}

	struct pcb_list* node = (*current);

	if (node == 0) {
		return 1;
	}	

	(*current) = node->next;

	kfree(node->proc);
	kfree(node);

	return 0;
}

struct pcb*
sched_fifo_next(struct pcb* p)
{
	if (p == 0) {
		if (proc_list == 0) {
			return 0;
		}	
		return proc_list->proc;
	}

	struct pcb_list* node = get_by_pid(p->id);

	if (node == 0) {
		return 0;
	}

	if (node->next == 0) {
		return proc_list->proc;
	} else {
		return node->next->proc;
	}
}

uint8_t
sched_fifo_tick(struct pcb* p)
{
	if (p == 0) {
		return 0;
	}

	struct pcb_list* node = get_by_pid(p->id);

	if (node == 0) {
		return 0;
	}

	if (p->runtime >= MAX_PROC_QUANTUM) {
		return 1;
	}

	return 0;

}

