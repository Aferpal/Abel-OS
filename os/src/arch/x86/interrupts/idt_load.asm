BITS 32
global idt_load
global sti
section .text

idt_load:
  push ebp
  mov ebp, esp

  mov eax, [ebp + 8]
  lidt [eax]

  mov esp, ebp
  pop ebp
  ret

sti:
  sti
  ret

section .note.GNU-stack noalloc noexec nowrite progbits

