#include "arch/x86/gdt.h"

struct tss_entry_struct global_tss = {
    .ss0 = (X86_GDT_KERNEL_DATA_INDEX * X86_GDT_ENTRY_SIZE) /* define the correc segment */
};

void
arch_prepare_userspace()
{
    gdt_set_entry(X86_GDT_USER_CODE_INDEX, 
                  0, 
                  0xfffff, 
                  X86_GDT_GRANULARITY_4K | X86_GDT_PROT_MODE,
                  X86_GDT_PRESENT | X86_GDT_USER | X86_GDT_CODE | X86_GDT_EXEC | X86_GDT_RW | X86_GDT_ACCESSED);

    gdt_set_entry(X86_GDT_USER_DATA_INDEX, 
                  0, 
                  0xfffff, 
                  X86_GDT_GRANULARITY_4K | X86_GDT_PROT_MODE,
                  X86_GDT_PRESENT | X86_GDT_USER | X86_GDT_DATA | X86_GDT_RW | X86_GDT_ACCESSED);

    gdt_set_entry(X86_GDT_TSS_INDEX, 
                  (ptr_t)&global_tss, 
                  sizeof(struct tss_entry_struct) - 1, 
                  X86_GDT_GRANULARITY_1B,
                  X86_GDT_PRESENT | X86_GDT_KERNEL | X86_GDT_TSS | X86_GDT_EXEC | X86_GDT_ACCESSED);

    gdt_load_tss();
}