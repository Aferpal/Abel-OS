section .text

global get_current_cr3

get_current_cr3:
  push ebp
  mov ebp, esp

  mov eax, cr3

  mov esp, ebp
  pop ebp
  ret

section .note.GNU-stack noalloc noexec nowrite progbits
