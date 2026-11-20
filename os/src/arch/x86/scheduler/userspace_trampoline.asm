global arch_userspace_trampoline

%define USER_DS ((4 * 8) | 3)

arch_userspace_trampoline:
    mov ax, USER_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iret