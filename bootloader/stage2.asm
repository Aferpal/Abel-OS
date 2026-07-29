bits 16

stage2_header:
MAGIC  dd 0x77684813

SIZE   dd stage2_end - stage2_start

ENTRY  dw stage2_entry

VERSION  dw 0x0100

stage2_start:

disk_packet:
P_SIZE  db 0x10
P_MAGIC  db 0x00
P_SECTORS  dw 0x0001
P_OFFSET  dw 0x0000
P_SEGMENT  dw 0x1000
P_LBADDR  dq 0x3

; Global Descriptor Table Definition
align 8
gdt:
  ; First NULL entry
  dq 0

  ; Second entry, code
  dq 0x00cf9a000000ffff

  ; Third entry, data
  dq 0x00cf92000000ffff

  ; Pre-reserved
  times 16 dq 0

gdt_end:

gdt_descriptor:
  dw gdt_end - gdt - 1
  dd gdt

kheader_cpy:
  KSIZE dd 0x0
  KENTRYOFF dd 0x0
  dw 0x08

stage2_entry:
  ; update segment registers
  mov ax, cs
  mov ds, ax

  ; can use LBA for loading segments?
  ; ah 0x41
  ; bx 0x55AA
  ; dl disk_id
  ; int 0x13
  ; carryflag = 1 -> error  
  mov ah, 0x41
  mov bx, 0x55AA
  mov dl, 0x80
  int 0x13
 
  jc halt_loop
  

  ; load the first sector and read the headers
  ; P_SIZE byte 0x10
  ; P_MAGIS byte 0x0
  ; P_SEGMENT:P_OFFSET [word:word] where to load
  ; P_SECTORS word n_sectors
  ; P_LBADDR byte sector to load
  mov byte [P_SIZE], 0x10
  mov byte [P_MAGIC], 0x0
  mov word [P_OFFSET], 0x0000
  mov word [P_SEGMENT], 0x1000
  mov word [P_SECTORS], 1
  mov byte [P_LBADDR], 0x2
 
  ; ah 0x42
  ; dl disk_id
  ; si pointer to struct
  ; int 0x13
  ; carryflag = 1 -> error
  mov ah, 0x42
  mov dl, 0x80
  mov si, disk_packet
  int 0x13

  jc bad_kheader
  





  ; read headers

  ; set segment for easy read
  mov ax, 0x1000
  mov ds, ax

  ; MAGIC dw 0x0-0x4
  mov ax, [0x0]
  cmp ax, 0x7178
  jne bad_magic
  mov ax, [0x2]
  cmp ax, 0x6636
  jne bad_magic
  
  ; size of the kernel 0x4-0x8
  mov ax, [0x4]
  mov word [cs:KSIZE], ax
  mov ax, [0x6]
  mov word [cs:KSIZE+2], ax
  
  ; +511/512 to obtain n_sectors
  add dword [cs:KSIZE], 511

  shr dword [cs:KSIZE], 9

  sub dword [cs:KSIZE], 1

  ; if high is greater than 0 cannot load that many
  cmp word [cs:KSIZE+2], 0x0
  jne big_size



  ; kernel entry offset 0x8-0xC
  mov ax, [0x8]
  mov [cs:KENTRYOFF], ax
  mov ax, [0xa]
  mov [cs:KENTRYOFF+2], ax

  add dword [cs:KENTRYOFF], 0x10000

  mov ax, cs
  mov ds, ax

  ; LOAD ALL THE KERNEL
  cmp dword [KSIZE], 0x0 ; if no sectors skip
  je prepare_protected_mode

  mov word [P_OFFSET], 0x0200 ; +512 bytes since already one sector has been loaded
  mov ax, [KSIZE]
  mov word [P_SECTORS], ax
  mov byte [P_LBADDR], 0x3 ; next 

  mov ah, 0x42
  mov dl, 0x80
  mov si, disk_packet
  int 0x13

  jc bad_load_sectors


prepare_protected_mode:
  ; activate line a20 for > 1MB of address
  in al, 0x92
  or al, 010b
  out 0x92, al
  
  mov ax, cs
  shl ax, 4

  add word [gdt_descriptor+2], ax

  ; we init the global descriptor table
  lgdt [gdt_descriptor]

  ; we enter the protected mode
  mov eax, cr0
  or eax, 1
  mov cr0, eax
  
  jmp far dword [KENTRYOFF]

bad_kheader:
  mov ah, 0x0e
  mov al, 'H'
  int 0x10 
  jmp halt_loop

bad_load_sectors:
  mov ah, 0x0e
  mov al, 'L'
  int 0x10
  jmp halt_loop

bad_magic:
  mov ah, 0x0e
  mov al, 'M'
  int 0x10
  jmp halt_loop

big_size:
  mov ah, 0x0e
  mov al, 'B'
  int 0x10
  jmp halt_loop

halt_loop:
  hlt
  jmp halt_loop  


times (512 - (($-$$) % 512)) % 512 db 0

stage2_end:
