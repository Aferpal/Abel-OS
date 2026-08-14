[org 0x7c00]
bits 16

stage_1_start: 
  jmp short stage_1_boot
  nop

; yea bro lets put a BPB 
times 90 - ( $ - $$ ) db 0



; =========================================================
; |                          DATA                         |
; =========================================================


disk db 0x0

align 4
disk_packet:
  P_SIZE    db 0x10
  P_MAGIC   db 0x00
  P_SECTORS dw 0x01
  P_OFFSET  dw 0x0
  P_SEGMENT dw 0x1000
  P_LBADDR  dq 0x02

jump_target:
stage2_entry_offset dw 12
  dw 0x07e0








; =========================================================
; |                          CODE                         |
; =========================================================


stage_1_boot:

  ; prepare cpu state, clear anything from the POST
  cli 
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov sp, 0x7000
  cld

  ; save disk id
  mov [disk], dl

  ; reset the disk, jic
  mov ah, 0x00
  mov dl, [disk]
  int 0x13

  ; does this disk have LBA? ( int 0x13, ah 0x41, dl = diskid, bx = 0x55AA )
  mov ah, 0x41
  mov bx, 0x55AA
  mov dl, [disk]
  int 0x13
  
  ; if carry flag is on, it is unsupported
  jc lba_capabilities_unsupported

  ; but if bx is not inverted, the operation may not have been successfull
  cmp bx, 0xaa55
  jne lba_capabilities_unsupported
  
  ; same with bit 0 of cx
  test cx, 0x01
  jz lba_capabilities_unsupported

  ; if we get to this point, lba should 100% be supported
  ; load 2 sectors, validate and jump

  ; we use ( int 0x13, ah 0x42, dl = diskid, si = disk_packet addr, 
  ;          P_SEGMENT:P_OFFSET = where to load in memory,
  ;          P_SECTORS = number of sectors to load (2),
  ;          P_LBADDR = what sector to start loading )

  mov ah, 0x42
  mov dl, [disk]
  mov byte [P_SIZE], 0x10     ; size of the struct, always 16B
  mov byte [P_MAGIC], 0x00    ; magic, must be 0x0
  mov word [P_OFFSET], 0x0    ; see next line
  mov word [P_SEGMENT], 0x7e0 ; we load at 0x7e0:0x0 which is 0x7e00
  mov word [P_SECTORS], 0x02  ; we load 2 sectors
  mov byte [P_LBADDR], 0x01  ; second sector, LBA starts at index 0
  mov si, disk_packet

  int 0x13

  ; if carry flag is on, it was not possible to bring that sectors to main
  ; memory. In that case we indicate fatal error and halt. 
  ; 
  ; TODO: make a retry loop with an arbitrary amount of retries in order 
  ;       to solve temporary errors 
  ;
  jc sector_read_fatal_error 

  jmp validate_stage2_headers

lba_capabilities_unsupported:
  mov ah, 0x08
  mov dl, [disk]
  xor di, di
  int 0x13
  
  ; in case that lba is not supported we can try using legacy chs
  ; for that we use ( int 0x13, ah 0x02, al = number of sectors to load (2),
  ;                   ch = cylinder number, cl = sector number, 
  ;                   dh = head number, dl = diskid, es:bx where to load )
  mov ah, 0x02    
  mov al, 0x02    ; we load 2 sectors
  mov ch, 0x00    ; cyl 0 ( we hope we don't find a 1 sector per cyl disk )
  mov cl, 0x02    ; second sector, CHS sectors start at index 1
  mov dh, 0x00    ; head 0, same as cylinder
  mov dl, [disk]  ; disk
  mov bx, 0x07e0  ; same logic as in LBADDR
  mov es, bx      ; 
  xor bx, bx      ;

  int 0x13

  ; same situation as with LBA error, same TODO
  ;
  jc sector_read_fatal_error
  
  jmp validate_stage2_headers

validate_stage2_headers:
  
  ; now we have to validate the stage2 format is expected
  ; we check the MAGIC number, should be 0x77684813
  
  ; the magic number must be located at offset 0 of stage2 sectors
  ; and will be 4 bytes

  mov ax, [0x7e00]
  cmp ax, 0x4813
  jne unsupported_stage2_invalid_magic

  mov ax, [0x7e00 + 2]
  cmp ax, 0x7768
  jne unsupported_stage2_invalid_magic

  ; if we've made this far, the magic is valid, we now get where to jump
  ; the entry_offset is in the header at offset 8 from start of stage2
  
  mov ax, [0x7e00 + 8]
  mov word [stage2_entry_offset], ax
    
  jmp far_jump_stage2 ; unnecesary jmp, added in case someone decides it
                      ; is a good idea to place code here

far_jump_stage2:
  
  ; finally we do the far jump to stage2
  ; in jump_target we have the address where to jump
  ; at offset 0 ( stage2_entry_offset ) we find an offset
  ; and at offset 2 the segment
  ; the offset was prepared in validate_stage2_headers

  mov word [jump_target + 2], 0x7e0 

  ; we save dl for stage2 to know the disk

  mov dl, [disk]

  ; and we finally jmp, this is stage1 end

  jmp far [jump_target]  

unsupported_stage2_invalid_magic:
  
  ; the magic read from stage2 headers is wrong, we print ME 
  ; ( Magic Error ) and halt
  
  mov al, 'M'
  mov ah, 0x0e
  int 0x10
  mov al, 'E'
  int 0x10

  jmp stage1_halt
  
sector_read_fatal_error:
   
  ; we were unable to load any sector of stage2, we print DE
  ; ( Disk Error ) and halt
  mov al, 'D'
  mov ah, 0x0e
  int 0x10
  mov al, 'E'
  int 0x10

  jmp stage1_halt


stage1_halt:
  hlt
  jmp stage1_halt



times 510 - ($-$$) db 0
dw 0xAA55
