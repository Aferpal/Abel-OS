#include "kernel/input/input.h"
#include "stdint.h"
#include "kernel/printk.h"

#define MAX_EVENTS_IN_QUEUE 16

struct input_queue {
    struct input_event data[MAX_EVENTS_IN_QUEUE];
    uint8_t head, tail;
};

static struct input_queue input_event_queue;

static void
input_queue_init(struct input_queue* queue)
{
    queue->head = -1;
    queue->tail = 0;
}

static uint8_t
input_queue_is_full(struct input_queue* queue)
{
    return queue->head == ((queue->tail + 1) % MAX_EVENTS_IN_QUEUE);
}

static uint8_t
input_queue_is_empty(struct input_queue* queue)
{
    return ((queue->head + 1) % MAX_EVENTS_IN_QUEUE ) == queue->tail;
}

static void
input_enqueue_event(struct input_queue* queue, input_event_t* event)
{
    if (input_queue_is_full(queue)) {
        printk(" [ INPUT_SYS ] PANIC, INPUT QUEUE IS FULL AND NO MORE IRQS CAN BE ATTENDED!\n");
        return;
    }

    queue->data[queue->tail].evid = event->evid;

    queue->data[queue->tail].key_evt.type = event->key_evt.type;

    queue->data[queue->tail].key_evt.value = event->key_evt.value;

    queue->tail++;
    queue->tail%=MAX_EVENTS_IN_QUEUE;
}

uint8_t
input_event_pop(struct input_event* ev_out)
{
    if (input_queue_is_empty(&input_event_queue)) {
        return 0;
    }

    ev_out->evid = input_event_queue.data[(input_event_queue.head + 1) % MAX_EVENTS_IN_QUEUE].evid;
    ev_out->key_evt.type = input_event_queue.data[(input_event_queue.head + 1) % MAX_EVENTS_IN_QUEUE].key_evt.type;
    ev_out->key_evt.value = input_event_queue.data[(input_event_queue.head + 1) % MAX_EVENTS_IN_QUEUE].key_evt.value;

    input_event_queue.head++;
    input_event_queue.head%=MAX_EVENTS_IN_QUEUE;

    return 1;

}



void
input_event_publish(input_event_t* event)
{
    // There are plenty options

    // 1. Simpler one. Act from here and dispatch a handler.

    // printk(...)

    // 2. Create an event queue that can be read by other processes

    input_enqueue_event(&input_event_queue, event);

    // 3. In combination with the one before, but wake a worker process to handle

    // signal....
}

void
input_event_key_publish(keyboard_input_key_e value, keyboard_input_type_e type)
{
    
    struct input_event ev = {
        .evid = EV_KEY,
        .key_evt = {
            .type = type,
            .value = value
        }
    };

    input_event_publish(&ev);

}


void
input_subsystem_init()
{   

    printk(" [ INPUT_MANAGER ] Setting up input queue\n");

    input_queue_init(&input_event_queue);

    printk(" [ INPUT_MANAGER ] Queue setup correctly\n");

}