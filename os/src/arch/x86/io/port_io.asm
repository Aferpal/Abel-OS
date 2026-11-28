section .text

global inb
global inw
global indw

global outb
global outw
global outdw

inb:
  push ebp
  mov ebp, esp
  
  mov dx, word [ebp + 8]
  in al, dx

  mov esp, ebp
  pop ebp
  ret

inw:
  push ebp
  mov ebp, esp
  
  mov dx, word [ebp + 8]
  in ax, dx

  mov esp, ebp
  pop ebp
  ret

indw:
  push ebp
  mov ebp, esp
  
  mov dx, word [ebp + 8]
  in eax, dx

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

outw:
  push ebp
  mov ebp, esp
  
  mov ax, word [ebp + 12]
  mov dx, word [ebp + 8]
  
  out dx, ax
  
  mov esp, ebp
  pop ebp
  ret

outdw:
  push ebp
  mov ebp, esp
  
  mov eax, dword [ebp + 12]
  mov dx, word [ebp + 8]
  
  out dx, eax
  
  mov esp, ebp
  pop ebp
  ret




section .note.GNU-stack noalloc noexec nowrite progbits
