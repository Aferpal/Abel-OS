bits 16

; As defined in the contract with stage1, first of all 
; we put the header with the specified format

stage2_header:

MAGIC dd 0x77684813

SIZE dd stage2_end - stage2_header

ENTRY dw stage2_entry

VERSION dw 0x0100

stage2_header_end:

stage2_start:


; boot disk id

disk db 0x0


; disk_packet for lba sector loading

disk_packet: 
P_SIZE    db 0x10
P_MAGIC   db 0x00
P_SECTORS dw 0x0001
P_OFFSET  dw 0x0400
P_SEGMENT dw 0x07e0
P_LBADDR  dq 0x0004


; remaining sectors
remaining_sectors_high dw 0x00
remaining_sectors_low  dw 0x00



; DISCLAIMER: In old versions all needed data would be found here
; however since stage2 is now responsible of loading itself, we decided
; to save as much space as possible. 1024 bytes are more than enough
; but it is better to keep things simple


; load_sectors_data

; has lba been checked ? 
lba_capabilities_check db 0x00

; if checked, are they enabled ?
has_lba_capabilities    db 0x00 


; has chs_geometry been stored?
chs_geometry_check     db 0x00

; chs_geometry struct
chs_geometry:
CHS_CYL                dw 0x00
CHS_SEC_PER_TRACK      db 0x00
CHS_HEADS              db 0x00

chs_current:
CHS_C_CYL              dw 0x00
CHS_C_SEC              db 0x00
CHS_C_HEADS            db 0x00


; ======================================================
; |                  FUNCTION DEFINITIONS              |
; ======================================================



; =======================================================================
; lba_capabilities:
; check and store in has_lba_capabilites if the [disk] has lba extensions
;
lba_capabilities:
  mov dl, [disk]
  mov ah, 0x41
  mov bx, 0x55AA
  int 0x13 

  ; we pre-store a false
  mov byte [has_lba_capabilities], 0x00
 
  jc lba_capabilities_exit
  
  cmp bx, 0xaa55
  jne lba_capabilities_exit
 
  test cx, 0x01
  jz lba_capabilities_exit

  ; if none of the fail conditions meet, we store true
  mov byte [has_lba_capabilities], 0x01

lba_capabilities_exit:
  mov byte [lba_capabilities_check], 0x01
  ret
; ========================================================================



; ========================================================================
; load_sectors_lba:
; dl, diskid
; ax, low 16 buts of number of sectors
; bx, high 16 bits of number of sectors
; cx, lba_addr to start
; on return, al is set to 0x01 on success, 0x00 on error
load_sectors_lba:

  mov [remaining_sectors_low], ax
  mov [remaining_sectors_high], bx

  ; we prepare the first packet
  mov byte [P_SIZE], 0x10
  mov byte [P_MAGIC], 0x00
  mov word [P_OFFSET], si
  mov word [P_SEGMENT], di
  mov word [P_LBADDR], cx
  mov word [P_LBADDR + 2], 0
  mov word [P_LBADDR + 4], 0
  mov word [P_LBADDR + 6], 0


load_sectors_lba_loop:

  mov ax, [remaining_sectors_low]
  mov dx, [remaining_sectors_high]

  ; if no more sectors exit
  or ax, dx
  jz load_sectors_lba_exit 

  cmp dx, 0
  jne .load_sectors_lba_127
  
  cmp ax, 127
  ja .load_sectors_lba_127

  ; in this case, we have less than 127 left
  mov [P_SECTORS], ax
  jmp load_sectors_lba_call
 
.load_sectors_lba_127:
  mov word [P_SECTORS], 127
  jmp load_sectors_lba_call

load_sectors_lba_call:
  mov ah, 0x42
  mov dl, [disk]
  mov si, disk_packet
  int 0x13

  jc load_sectors_lba_fatal_error

  ; we need to increment [ P_LBADDR ] and [ P_SEGMENT:P_OFFSET ]
  mov ax, [P_SECTORS]
  
  ; inc lbaddr
  add word [P_LBADDR], ax
  adc word [P_LBADDR + 2], 0
  adc word [P_LBADDR + 4], 0
  adc word [P_LBADDR + 6], 0

  ; decrease remaining
  sub [remaining_sectors_low], ax
  sbb word [remaining_sectors_high], 0x00

  ; increase destination
  ; since we augment only the segment
  ; segment = segment + ax*512/16 ( since segments mult by 16 )
  ; thats   = segment + ax * 32 = segment + ax << 5
  
  shl ax, 5
  
  add [P_SEGMENT], ax
  mov word [P_OFFSET], 0


  jmp load_sectors_lba_loop

load_sectors_lba_fatal_error:
  mov al, 0x00
  ret

load_sectors_lba_exit:
  mov al, 0x01
  ret
; ========================================================================



; ========================================================================

; chs_get_geometry:
; no params
; al 0x01 on success, 0x00 on failure and ah error code

; ========================================================================
chs_get_geometry:

  ; int 0x13 ah 0x08 to get geometry from bios
  ; dl must be the disk id
  ; on failure carry flag is on
  ; on success cl & 0x3F is number of sectors per track
  ; dh is number of heads ( 0-based, important!! )
  ; cl[7-6]:ch is number of cilinders, 0-based
  ;
  mov ah, 0x08
  mov dl, [disk]
  xor di, di     ; legacy recommendation
  int 0x13

  jc chs_get_geometry_fail

  mov al, cl
  and al, 0x3F
  mov byte [CHS_SEC_PER_TRACK], al

  add dh, 0x01
  mov byte [CHS_HEADS], dh 

  mov al, cl   ; copy cl
  shr al, 6    ; shift 6 bits ( now [7-6] are [1-0] )
  and al, 0x03 ; defensive mask, ensure only [1-0] contain data
  mov ah, al   ; move lower to higher, now they are [9-8] in ax
  mov al, ch   ; copy lower to ax lower
  add ax, 0x01 ; add 1 to ax
  mov word [CHS_CYL], ax 

  mov byte [chs_geometry_check], 0x01

  mov al, 0x01
  ret


chs_get_geometry_fail:
  ; ah contains bios error code
  mov al, 0x00
  ret
; ========================================================================




; ========================================================================
; load_sectors_chs:
; dl, diskid
; ax, low 16 buts of number of sectors
; bx, high 16 bits of number of sectors
; cx, lba_addr to start
; di:si dest addr
; on return, al is set to 0x01 on success, 0x00 on error
load_sectors_chs:

  mov [remaining_sectors_low], ax
  mov [remaining_sectors_high], bx
  mov word [P_SEGMENT], di
  mov word [P_OFFSET], si

  ; first we check, is chs geometry already in memory?
  cmp byte [chs_geometry_check], 0x00
  ; if so start the loop
  jne load_sectors_chs_lba_to_chs

  ; otherwise we have to bring it to memory
  push ax
  push bx
  push cx
  push dx
  push di
  push si

  call chs_get_geometry

  cmp al, 0x01
  je .load_sector_chs_geometry_ok
  add sp, 12
  mov ah, 0x00
  ret

.load_sector_chs_geometry_ok:
  pop si
  pop di
  pop dx
  pop cx
  pop bx
  pop ax

  jmp load_sectors_chs_lba_to_chs

load_sectors_chs_lba_to_chs:

  ; after making sure our chs struct is correct, we have to 
  ; transform our lba into a chs value
  mov bx, dx ; save for later
  
  mov ax, cx
  xor dx, dx
  movzx di, byte [CHS_SEC_PER_TRACK]
  div di

  add dx, 0x01

  mov byte [CHS_C_SEC], dl

  xor dx, dx
  movzx di, byte [CHS_HEADS]
  div di

  mov byte [CHS_C_HEADS], dl
  mov word [CHS_C_CYL], ax

  mov dx, bx ; recover dx ( has dl, disk )

  jmp load_sectors_chs_loop
  
load_sectors_chs_loop:

  mov ax, [remaining_sectors_low]
  mov dx, [remaining_sectors_high]

  ; if no more sectors exit
  or ax, dx
  jz load_sectors_chs_exit 

load_sectors_chs_call:
  mov ah, 0x02
  mov al, 0x01
  mov ch, byte [CHS_C_CYL]
  mov cl, byte [CHS_C_CYL + 1]
  and cl, 0x03
  shl cl, 6
  or cl, byte [CHS_C_SEC]
  mov dh, byte [CHS_C_HEADS]
  mov es, [P_SEGMENT]
  mov bx, [P_OFFSET]
  int 0x13

  jc load_sectors_chs_fatal_error

  ; decrease remaining
  sub word [remaining_sectors_low], 0x01
  sbb word [remaining_sectors_high], 0

  ; increase destination
  ; since we augment only the segment
  ; segment = segment + ax*512/16 ( since segments mult by 16 )
  ; thats   = segment + ax * 32 = segment + ax << 5
  
  add word [P_SEGMENT], 32
  mov word [P_OFFSET], 0

  ; now we increment our chs values

  add byte [CHS_C_SEC], 0x01
  mov al, [CHS_C_SEC]
  cmp al, [CHS_SEC_PER_TRACK]
  jbe load_sectors_chs_loop

  mov byte [CHS_C_SEC], 0x01
  add byte [CHS_C_HEADS], 0x01
  mov al, [CHS_C_HEADS]
  cmp al, [CHS_HEADS]
  jb load_sectors_chs_loop
  
  mov byte [CHS_C_HEADS], 0x00
  add word [CHS_C_CYL], 0x01
  mov ax, [CHS_C_CYL]
  cmp ax, [CHS_CYL]
  
  jb load_sectors_chs_loop

  ; if we are here, we reached the limit of the disk
  jmp load_sectors_chs_fatal_error

load_sectors_chs_fatal_error:
  mov al, 0x00
  ret

load_sectors_chs_exit:
  mov al, 0x01
  ret
; ========================================================================





; ========================================================================
; load_sectors: 
; dl, diskid
; ax, low 16 bits of number of sectors
; bx, high 16 bits of number of sectors
; cx, lba_addr to start ( if chs is needed it is handled inside the call )
; si, offset
; di, segment where to load
; on return, al is set to 0x01 on success, 0x00 on error

load_sectors:
   
  ; we first check if zero sectors were asked, in that case return success
  cmp bx, 0x00
  jne load_sectors_n_sectors_valid

  cmp ax, 0x00
  jne load_sectors_n_sectors_valid

  mov al, 0x01
  ret

load_sectors_n_sectors_valid:

  ; if we got to here we have something to load
  ; first check if lba have been asked
  cmp byte [lba_capabilities_check], 0x00
  jne load_sectors_lba_capabilities_checked

  push ax
  push bx
  push cx
  push si
  push di
  push dx

  call lba_capabilities

  pop dx
  pop di
  pop si
  pop cx
  pop bx
  pop ax

load_sectors_lba_capabilities_checked:
  
  ; now we check if they are or they are not enabled and call the subfunction
  cmp byte [has_lba_capabilities], 0x00
  je load_sectors_lba_unsupported
  
  ; if we are here, lba is supported
  ; we call load_sectors_lba
  ; no need to save a thing since it is the last call
  call load_sectors_lba
  
  cmp al, 0x01
  je load_sectors_end
  jmp load_sectors_fail

load_sectors_lba_unsupported:

  ; if lba is not supported try to use chs
  call load_sectors_chs

  cmp al, 0x01
  je load_sectors_end
  jmp load_sectors_fail

load_sectors_end:
  mov al, 0x01
  ret
load_sectors_fail:
  mov al, 0x00
  ret
; ========================================================================










; ======================================================
; |                 END OF FUNC DEFS                   |
; =====================================================





stage2_entry:

  ; all contract defines is dl is given and cs is set correctly
  ; so we ourselves set a coherent cpu segment state

  mov ax, cs
  mov ds, ax
  mov es, ax
  
  ; first of all save our disk id
  mov [disk], dl

  ; we enter the bootstrap section. We have to check our own size knowing 
  ; two full sectors have already been loaded. If we are bigger than 
  ; two sectors, proceed to load the rest of them

  ; the size is stored in [ SIZE ]
  ; we first add 511, this way we can later get the number of sectors
  ; ( we are ceiling the value at modulus 512 )

  ; move to registers for faster performance ( rather than using mem, 
  ; ofc caches exist but who knows if someones old pc does not have
  ; a cache, we need to be considered )
  mov ax, [SIZE]
  mov dx, [SIZE+2]
  
  add ax, 511
  adc dx, 0

  ; now we divide by 512, which is the same as shifting 9 times

  mov cx, 9

shift_size_divide_by_512:
  shr dx, 1
  rcr ax, 1
  loop shift_size_divide_by_512

  ; finally we sub 2 since two sectors have already been loaded
  ; ( be carefull, maybe size is only one sector, we should check before 
  ; and if not sub 2 )
  
  ; we check, if high bits are not zero we definitively have sectors to load
  cmp dx, 0x00
  jne sectors_pending_to_load

  ; if ax is greater than 2, then same situation
  cmp ax, 0x02
  ja sectors_pending_to_load
  
  ; otherwise full stage2 has been loaded and we can skip this
  jmp stage2_post_bootstrap_entry

sectors_pending_to_load:
  sub ax, 2
  sbb dx, 0

  ; we should store this, jic
  mov [remaining_sectors_low], ax
  mov [remaining_sectors_high], dx

  ; now we should load it
  ; DISCLAIMER: In older versions, all the logic would be spread, now 
  ; everything stays inside load_sectors function. Here everything is handled,
  ; from lba capabilites to possibly needed loops. 
  ;
  ; call to load_sectors
  ; dl, diskid
  ; ax, low 16 bits of number of sectors
  ; bx, high 16 bits of number of sectors
  ; cx, lba_addr to star ( if chs is needed it is handled inside the call )
  ; on return, al is set to 0x01 on success, 0x00 on error
  mov dl, [disk]
  mov ax, [remaining_sectors_low]
  mov bx, [remaining_sectors_high]
  mov cx, 0x03 ; we use 3rd sector, stage1 is 1 sector and 2 sectors of stage2 are loaded
  mov si, 0x000
  mov di, 0x820 ; we load after our two sectors
  call load_sectors

  cmp al, 0x00

  je stage2_bootstrap_load_sectors_fatal_error

  ; once everything is loaded up, we can keep it up
  jmp stage2_post_bootstrap_entry


stage2_bootstrap_load_sectors_fatal_error:
  mov al, 'S'
  mov ah, 0x0e
  int 0x10
  mov al, 'B'
  int 0x10
  mov al, 'E'
  int 0x10
  hlt
  jmp stage2_bootstrap_load_sectors_fatal_error


















; =========================================================================
; | BIG SPACE TO MARK THE DIFFERENCE BETWEEN BOOTSTRAP AND POST BOOTSTRAP |
; =========================================================================














stage2_post_bootstrap_start:

; as said before, now all data not needed in bootstrap is stored here
; in order to save space

; disk packet is reused from bootstrap

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

align 4
boot_info_struct:
  boot_magic           dd 0x21012020
  
  e820_mmap_buffer_ptr dd 0x0 
  e820_mmap_entries    dw 0x0
  padding              dw 0x0
  
  bootstrap_start      dd 0x07c00
  bootstrap_end        dd e820_buffer

; ======================================================
; load_mmap: 
; es:di should point to the buffer where to load
; on success al = 0x01, 0x00 on error
; and on success e820_mmap_entries contains number of entries
;
load_mmap:
 
  ; ebx set to 0 to start
  mov word [e820_mmap_entries], 0x00

  mov word [e820_mmap_buffer_ptr], di

  mov ax, es
  mov dx, es

  shl ax, 4
  shr dx, 12

  add word [e820_mmap_buffer_ptr], ax
  adc word [e820_mmap_buffer_ptr + 2], dx
  
  xor ebx, ebx

  .e820_loop:
    mov edx, 0x534D4150
    mov eax, 0xE820
    mov ecx, 24
    int 0x15

    jc .e820_error
    cmp eax, 0x534D4150
    jne .e820_error

    cmp ecx, 0
    je .e820_invalid_entry

    add di, 24
    inc word [e820_mmap_entries]

  .e820_invalid_entry:
    test ebx, ebx
    jne .e820_loop

  .e820_done:
    mov al, 0x01
    ret
  
  .e820_error:
    mov al, 0x00
    ret

; ======================================================

stage2_post_bootstrap_entry:
  
  ; after loading ourselves we can start the work
  ; at first we may run the e820 bios call to retrieve
  ; the memory map

  ; point es:di to destination buffer
  mov di, e820_buffer
  
  call load_mmap

  cmp al, 0x01

  jne stage2_mmap_fatal_error

  ; after loading ourselves up we load the kernel
  ; we first load one single sector and read the header
  
  ; calculate lba start of kernel
  mov ax, [SIZE]
  mov dx, [SIZE+2]

  add ax, 511
  adc dx, 0
  
  mov cx, 9

shift_size_for_kernel_lba:
  shr dx, 1
  rcr ax, 1
  loop shift_size_for_kernel_lba

  ; we have to add 1 for the stage1 which is 1 block
  add ax, 1
  adc dx, 0

  
  mov cx, ax     ; currently ignoring if lba is higher than 2^16 :)
                 ; like cmon who needs such a stage2 
  mov dl, [disk]
  mov ax, 0x01
  mov bx, 0x00
  mov si, 0x00
  mov di, 0x1000

  call load_sectors

  cmp al, 0x01
  jne stage2_kernel_load_sectors_fatal_error



  ; read headers

  ; set segment for easy read
  mov ax, 0x1000
  mov ds, ax

  ; MAGIC dw 0x0-0x4
  mov ax, [0x0]
  cmp ax, 0x7178
  jne stage2_kernel_header_fatal_error
  mov ax, [0x2]
  cmp ax, 0x6636
  jne stage2_kernel_header_fatal_error
  
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
  jne stage2_kernel_header_fatal_error



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

  ; recalculate once more our size in sectors
  mov ax, [SIZE]
  mov dx, [SIZE+2]

  add ax, 511
  adc dx, 0

  mov cx, 9

shift_size_for_kernel_lba_2:
  shr dx, 1
  rcr ax, 1
  loop shift_size_for_kernel_lba_2

  ; we have to add 2 for the stage1 which is 1 block and 1 for k_header
  add ax, 2
  adc dx, 0


  mov cx, ax     ; currently ignoring if lba is higher than 2^16 :)
                 ; like cmon who needs such a stage2 
  mov dl, [disk]
  mov ax, word [KSIZE]
  mov bx, word [KSIZE + 2]
  mov si, 0x0200 
  mov di, 0x1000

  call load_sectors

  cmp al, 0x01
  jne stage2_kernel_load_sectors_fatal_error



prepare_protected_mode:
  in al, 0x92
  or al, 010b
  out 0x92, al


  mov ax, cs
  shl ax, 4

  add word [gdt_descriptor + 2], ax

  lgdt [gdt_descriptor]

  mov eax, cr0
  or eax, 1
  mov cr0, eax

  
  mov eax, cs
  shl eax, 4

  add [bootstrap_end], eax

  add eax, boot_info_struct

  jmp far dword [KENTRYOFF]

stage2_kernel_header_fatal_error:
  mov al, 'K'
  mov ah, 0x0e
  int 0x10
  mov al, 'H'
  int 0x10
  mov al, 'E'
  int 0x10
  hlt
  jmp stage2_kernel_header_fatal_error

stage2_kernel_load_sectors_fatal_error:
  mov al, 'S'
  mov ah, 0x0e
  int 0x10
  mov al, 'K'
  int 0x10
  mov al, 'E'
  int 0x10

  hlt
  jmp stage2_kernel_load_sectors_fatal_error

stage2_mmap_fatal_error:
  mov al, 'M'
  mov ah, 0x0e
  int 0x10
  
  mov al, 'P'
  int 0x10

  mov al, 'E'
  int 0x10
  
  hlt
  jmp stage2_mmap_fatal_error

align 4
e820_buffer: ; this is just a tag meaning the end of real code. 
             ; we do not need to account the padding 

times (512 - (($-$$) % 512 )) % 512 db 0

stage2_end:
