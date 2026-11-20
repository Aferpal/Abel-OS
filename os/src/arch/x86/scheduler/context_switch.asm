global arch_switch_context

extern global_tss

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

  ; next->kstack_ptr
  mov eax, [edx + 12]

  ; if kstack_ptr is 0 then this is a kthread
  ; in which case there ain't no need to change tss
  cmp eax, 0
  je noloadtss

  ; load the tss
  mov [global_tss + 4], eax

noloadtss:

  ; mov eax, next->vmm
  mov eax, [edx + 24]

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
