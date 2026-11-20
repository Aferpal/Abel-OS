#include "arch/x86/gdt.h"

extern struct gdt_entry kernel_gdt[];
extern void load_tss();

void
gdt_set_entry(uint8_t entry_index, uint32_t base, uint32_t limit, uint8_t flags, uint8_t access)
{
    kernel_gdt[entry_index].base_low    = (base & 0xFFFF);
    kernel_gdt[entry_index].base_mid    = (base >> 16) & 0xFF;
    kernel_gdt[entry_index].base_high   = (base >> 24) & 0xFF;

    kernel_gdt[entry_index].limit_low   = (limit & 0xFFFF);
    kernel_gdt[entry_index].limit_high  = (limit >> 16) & 0x0F;

    kernel_gdt[entry_index].flags       = flags & 0x0F;
    kernel_gdt[entry_index].access_byte = access;
}

void
gdt_load_tss()
{
    load_tss();
}