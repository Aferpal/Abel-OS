global arch_switch_context

; at esp, as always eip
; at esp + 4, struct pcb* prev
; at esp + 8, struct pcb* next
arch_switch_context:
  ; push callee-saved registers according to c
  
  pushfd
  push ebp
  push ebx
  push esi
  push edi

  ; struct pcb* prev
  mov eax, [esp + 24] 
  ; prev->stack_ptr = esp
  mov [eax + 8], esp

  ; struct pcb* next
  mov edx, [esp + 28]

  ; mov eax, next->vmm
  mov eax, [edx + 20]

  ; mov eax, next->vmm->arch
  mov eax, [eax + 4]

  ; mov eax, next->vmm->arch.cr3
  mov eax, [eax + 0]

  ; mov ecx, esp
  mov ecx, [edx + 8]

  mov cr3, eax ; now virtual space is switched
  mov esp, ecx ; now stack is pointer somewhere valid 
 		; we can pop safely

  pop edi
  pop esi
  pop ebx
  pop ebp
  popfd  
  ret

section .note.GNU-stack noalloc noexec nowrite progbits
