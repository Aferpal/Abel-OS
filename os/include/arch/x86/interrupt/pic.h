#ifndef _X86_PIC_H_
#define _X86_PIC_H_

#include <stdint.h>
/* PIC I/O ports */

#define PIC1_COMMAND       0x20
#define PIC1_DATA          0x21

#define PIC2_COMMAND       0xA0
#define PIC2_DATA          0xA1


/* PIC commands */

#define PIC_CMD_EOI        0x20


/* ICW1 */

#define ICW1_ICW4          0x01
#define ICW1_SINGLE        0x02
#define ICW1_INTERVAL4     0x04
#define ICW1_LEVEL         0x08
#define ICW1_INIT          0x10


/* ICW3 */

#define PIC_CASCADE_IRQ        2
#define ICW3_MASTER_SLAVE2     (1 << PIC_CASCADE_IRQ)
#define ICW3_SLAVE_ID2         2


/* ICW4 */

#define ICW4_8086          0x01
#define ICW4_AUTO          0x02
#define ICW4_BUF_SLAVE     0x08
#define ICW4_BUF_MASTER    0x0C
#define ICW4_SFNM          0x10


/* Vector remapping */

#define PIC1_VECTOR_OFFSET 32
#define PIC2_VECTOR_OFFSET 40


/* IRQ layout */

#define PIC_MASTER_IRQS    8
#define PIC_SLAVE_IRQ_BASE 8

void pic_send_eoi(uint8_t);

void pic_remap();

void pic_mask_all();

void pic_unmask_all();

void pic_set_mask(uint8_t);

#endif