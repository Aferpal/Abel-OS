section .text

global reload_page_directory

reload_page_directory:
  push ebp
  mov ebp, esp

  mov eax, cr3
  mov cr3, eax

  mov esp, ebp
  pop ebp
  ret

section .note.GNU-stack noalloc noexec nowrite progbits
