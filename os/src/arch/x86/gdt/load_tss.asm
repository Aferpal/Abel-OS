global load_tss

load_tss:
    mov ax, (5*8) | 0
    ltr ax
    ret



section .note.GNU-stack noalloc noexec nowrite progbits
