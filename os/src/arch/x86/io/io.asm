section .text

global inb
global outb

inb:
  push ebp
  mov ebp, esp
  
  mov dx, word [ebp + 8]
  in al, dx

  mov esp, ebp
  pop ebp
  ret

outb:
  push ebp
  mov ebp, esp
  
  mov al, byte [ebp + 12]
  mov dx, word [ebp + 8]
  
  out dx, al
  
  mov esp, ebp
  pop ebp
  ret


section .note.GNU-stack noalloc noexec nowrite progbits
