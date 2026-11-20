#ifndef _X86_GDT_H_
#define _X86_GDT_H_

#include "stdint.h"

/* FLAGS[0] GRANULARITY */
#define X86_GDT_GRANULARITY_1B 0x0
#define X86_GDT_GRANULARITY_4K 0x8

/* FLAGS[1] MODE */
#define X86_GDT_REAL_MODE      0x0
#define X86_GDT_PROT_MODE      0x4

/* FLAGS[2] LONG */
#define X86_GDT_LONG_MODE      0x2




/* ACCESS BYTE[0] PRESENT */
#define X86_GDT_NONPRESENT     0x00
#define X86_GDT_PRESENT        0x80

/* ACCESS BYTE[1-2] PRIVILEGE */
#define X86_GDT_KERNEL         0x00
#define X86_GDT_USER           0x60

/* ACCESS BYTE[3] TSS */
#define X86_GDT_TSS            0x00
#define X86_GDT_CODE           0x10
#define X86_GDT_DATA           0x10


/* ACCESS BYTE[4] EXECUTABLE */
#define X86_GDT_EXEC           0x08

/* ACCESS BYTE[5] DC */
#define X86_GDT_PRIV_STRICT    0x00
#define X86_GET_PRIV_RELAX     0x04

/* ACCESS BYTE[6] R/W */
#define X86_GDT_RONLY          0x00
#define X86_GDT_RW             0x02

/* ACCESS BYTE[7] ACCESSES */
#define X86_GDT_NACCESSED      0x00
#define X86_GDT_ACCESSED       0x01

#define X86_GDT_ENTRY_SIZE        8

#define X86_GDT_KERNEL_CODE_INDEX 1
#define X86_GDT_KERNEL_DATA_INDEX 2
#define X86_GDT_USER_CODE_INDEX   3
#define X86_GDT_USER_DATA_INDEX   4
#define X86_GDT_TSS_INDEX         5

struct __attribute__((packed)) gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access_byte;
    uint8_t  limit_high : 4;
    uint8_t  flags      : 4;
    uint8_t  base_high;
};

struct tss_entry_struct {
	uint32_t prev_tss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
	uint32_t esp0;     // The stack pointer to load when changing to kernel mode.
	uint32_t ss0;      // The stack segment to load when changing to kernel mode.
	uint32_t esp1; // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
}__attribute__((packed));

void gdt_set_entry(uint8_t entry_index, uint32_t base, uint32_t limit, uint8_t flags, uint8_t access);

void gdt_load_tss();

#endif