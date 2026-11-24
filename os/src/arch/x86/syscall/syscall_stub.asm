BITS 32

; SYSCALL STUB
extern arch_syscall_dispatch
global syscall_stub
syscall_stub:
  push ds
  push es
  push fs
  push gs

  pusha 

  mov eax, 0x10
  mov ds, eax
  mov es, eax
  mov fs, eax
  mov gs, eax

  push esp
  call arch_syscall_dispatch
  add esp, 4

  popa

  pop gs
  pop fs
  pop es
  pop ds

  iret

section .note.GNU-stack noalloc noexec nowrite progbits