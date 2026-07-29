[BITS 32]

section .header

extern kernel_size
extern kernel_entry_offset
kernel_headers:
  dd 0x66367178
  dd kernel_size
  dd kernel_entry_offset
  dw 0x0100

section .text

global kernel_entry
extern kernel_main

kernel_entry:
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov esp, stack_top
  call kernel_main

section .bss

align 16
stack:
  resb 4096
stack_top:


kernel_end:

section .note.GNU-stack noalloc noexec nowrite progbits
