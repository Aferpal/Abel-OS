BITS 32

%macro ISR_NOERROR 1

global isr%1
isr%1:
  push 0
  push %1
  jmp common_stub

%endmacro

%macro ISR_ERROR 1

global isr%1
isr%1:
  push %1
  jmp common_stub

%endmacro

%macro IRQ_STUB 1

global irq%1
irq%1:
  push 0
  push 32+%1
  jmp common_stub

%endmacro

section .text 

; intel exceptions
ISR_NOERROR 0
ISR_NOERROR 1
ISR_NOERROR 2
ISR_NOERROR 3
ISR_NOERROR 4
ISR_NOERROR 5
ISR_NOERROR 6
ISR_NOERROR 7
ISR_ERROR 8
ISR_NOERROR 9
ISR_ERROR 10
ISR_ERROR 11
ISR_ERROR 12
ISR_ERROR 13
ISR_ERROR 14
ISR_NOERROR 15
ISR_NOERROR 16
ISR_ERROR 17
ISR_NOERROR 18
ISR_NOERROR 19
ISR_NOERROR 20
ISR_NOERROR 21
ISR_NOERROR 22
ISR_NOERROR 23
ISR_NOERROR 24
ISR_NOERROR 25
ISR_NOERROR 26
ISR_NOERROR 27
ISR_NOERROR 28
ISR_NOERROR 29
ISR_NOERROR 30
ISR_NOERROR 31

; IRQs
IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15

; DEFAULT
global default_isr
default_isr:
  push 0
  push 100
  jmp common_stub

extern interrupt_dispatch

common_stub:

  pusha

  mov eax, esp
  push eax

  call interrupt_dispatch

  ; pop the eax we passed as argument for c
  add esp, 4

  popa

  ; pop the vector we passed before and the error_code
  add esp, 8

  iret


section .note.GNU-stack noalloc noexec nowrite progbits
