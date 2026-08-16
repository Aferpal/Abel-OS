ENTRY(kernel_entry)

SECTIONS
{
    . = 0x10000;
    
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

    kernel_end = .;
}
