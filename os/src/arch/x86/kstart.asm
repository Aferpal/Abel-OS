[BITS 32]

section .header

extern kernel_size
extern kernel_entry_offset
kernel_headers:
  dd 0x66367178
  dd kernel_size
  dd kernel_entry_offset
  dw 0x0100

section .data

global kernel_gdt
align 8
kernel_gdt:
  dq 0x0 ; First mandatory null entry
  dq 0x00cf9a000000ffff ; code
  dq 0x00cf92000000ffff ; data
  times 16 dq 0
kernel_gdt_end:

kernel_gdt_descriptor:
  dw kernel_gdt_end - kernel_gdt - 1
  dd kernel_gdt

section .text

global kernel_entry
extern kernel_main
extern kernel_end
extern kernel_virt_start
extern kernel_phys_start

kernel_entry:
  mov bx, 0x10
  mov ds, bx
  mov es, bx
  mov fs, bx
  mov gs, bx
  mov ss, bx
  
  mov esp, stack_top
  sub esp, kernel_virt_start
  add esp, kernel_phys_start

  ; in ax we have pointer to boot_info
  ; we copy the param to save it and free the register
  mov ebx, boot_param_addr
  sub ebx, kernel_virt_start
  add ebx, kernel_phys_start
  mov [ebx], eax

  ; kernel is linked to high addresses (0xC0000000)
  ; but at this point we are loaded somewhere else (0x10000)
  ; and no paging is enabled. For that reason we cannot 
  ; use memory addresses directly, we need to first 
  ; create the virtual page directory/tables for the boot
  ; later the full vmm will be initialized with the final
  ; virtual memory arrangement

  ; to keep things simple for the pmm, we are going to keep
  ; a simple approach, the first 4MB will have identity mapping
  ; this way both the kernel, the boot page tables and the future
  ; pmm bitmap will have enough space to be addressed.
  
  ; On the other hand, 0xC0000000 -> 0xFFFFFFFF will also be mapped.
  ; The idea is that the last pd entry points to itself, and the 
  ; kernel is also mapped so we can jump to standard c code

  ; we must initialize the first pages
  ; they will be allocated strictly after the kernel
  ; since that is the first mandatory structure we will need ( for now )
  ; ** REMEMBER ** that the gdt must also be somewhere. It could be a good 
  ; to map it first of all 

  ; we load in ebx the phy addr of the kernel end
  ; here we first store the page directory 
  
  mov ebx, kernel_end
  sub ebx, kernel_virt_start
  add ebx, kernel_phys_start
  ; we clear the full page directory to 0s

  cld
  xor eax, eax
  mov edi, ebx
  mov ecx, 1024
  rep stosd

  ; now we calc on ecx the start of page table 1
  mov ecx, ebx
  add ecx, 4096

  ; and put this in first page directory entry
  mov [ebx], ecx
  or dword [ebx], 0x03

  ; now we do an identity mapping of first 4MB in page table 1
  mov edi, 0x3
first_page_table_fill_loop:
  mov [ecx], edi
  add ecx, 4
  add edi, 0x1000
  cmp edi, 0x400000
  jb first_page_table_fill_loop

  ; we also want create a page for the core structures, mapped
  ; at 0xC0000000
  mov [ebx + 0xC00], ecx
  or dword [ebx + 0xC00], 0x03

  ; we clear the page, nothing is mapped yet, we only want the 
  ; page table to be there
  cld
  xor eax, eax
  mov esi, ecx
  mov ecx, 1024
  rep stosd

  mov ecx, esi
  add ecx, 0x1000
  ; we also map the kernel to the known address 0xc0400000
  ; to get its physical location we use kernel_phy_addr

  mov [ebx + 0xC04], ecx
  or dword [ebx + 0xC04], 0x03

  mov edi, 0x03
kernel_page_table_fill_loop:
  mov [ecx], edi
  add dword [ecx], kernel_phys_start
  add ecx, 4
  add edi, 0x1000
  cmp edi, 0x400000
  jb kernel_page_table_fill_loop

  ; finally we map the last entry to itself
  ; 0xffc is 0x2ff ( 1023 ) * 4
  mov [ebx + 0xffc], ebx
  or dword [ebx + 0xffc], 0x03

  ; at this point, the tables are ready. 
  mov cr3, ebx

  mov eax, cr0
  or eax, 0x80000001
  mov cr0, eax
  
  ; now that paging is enabled we reload our gdt
  lgdt [kernel_gdt_descriptor]
  
  ; we also reload our stack 
  mov esp, stack_top

  ; and prepare the boot_params for kernel
  mov eax, [boot_param_addr]
  push eax ; push the pointer

  ; perform the call this way to jump to high addresses
  mov eax, kernel_main
  call eax

section .bss

align 16

boot_param_addr:
  resb 4

stack:
  resb 4096
stack_top:


section .note.GNU-stack noalloc noexec nowrite progbits
