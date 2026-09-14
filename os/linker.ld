ENTRY(kernel_entry)

SECTIONS
{
    . = 0xC0400000;
    
    kernel_virt_start = .;

    kernel_phys_start = 0x00010000;

    kernel_start = .;

    .header :
    {
	    *(.header)
    }
  
    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }


    kernel_file_end = .;
    kernel_size = kernel_file_end - kernel_start;
    kernel_entry_offset = kernel_entry - kernel_start;

    .bss :
    {
	    *(.bss*)
    }

    . = ALIGN(4096);
    kernel_end = .;

}
