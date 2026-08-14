org 0x7c00
start: jmp boot

disk db 0x0

remaining_sectors_high dw 0x00
remaining_sectors_low dw 0x00

cur_cil db 0x0
prop_cil db 0x0
cur_sec db 0x0
prop_sec db 0x0
cur_head db 0x0
prop_head db 0x0

disk_packet:
  P_SIZE    db 0x10
  P_MAGIC   db 0x00
  P_SECTORS dw 0x01
  P_OFFSET  dw 0x0
  P_SEGMENT dw 0x1000
  P_LBADDR  dq 0x2

jump_target:
  dw 12 
  dw 0x07e0

; ================================= LBA UTILS ===================================

; uint8_t lba_capabilities(uint8_t disk)
; *parameter* **disk** passed through al, disk identifier, i.e. 0x80
; return value on al, 0x01 on true, 0x00 on false

lba_capabilities:
  mov dl, al
  mov ah, 0x41
  mov bx, 0x55AA
  int 0x13
  
  mov al, 0x01

  ; if capabilites are on, exit
  jnc lba_capabilities_exit

  ; otherwise set return value to 0
  mov al, 0x00

lba_capabilities_exit:  
  ret

; uint8_t lba_load_sector(uint8_t disk, uint8_t lba_addr, uint16_t n_sectors, uint16_t segment, uint16_t offset)
; **disk** as ah disk id
; **lba_addr** as al block address in disk
; **n_sectors** as si
; **segment** as di
; **offset** as dx
; return on al, 0x01 on success
lba_load_sector:
  mov byte [P_SIZE], 0x10
  mov byte [P_MAGIC], 0x0
  mov word [P_OFFSET], dx
  mov word [P_SEGMENT], di
  mov word [P_SECTORS], si
  mov byte [P_LBADDR], al

  mov dl, ah
  
  mov ah, 0x42
  mov si, disk_packet
  int 0x13

  mov al, 0x01

  jnc lba_load_sector_exit

  mov al, 0x00

lba_load_sector_exit:
  ret



; ===================================== CHS UTILS ===============================

; uint8_t get_chs_info 
; param in al -> disk id
; on return bl -> drive type
;           dh -> n_heads
;           cx -> bit 0-5 is sectors, 6-15 cylinders
;           al -> 0x01 success
get_chs_info:
  mov dl, al
  mov ah, 0x08
  xor di, di
  int 0x13

  mov al, 0x01

  jnc get_chs_info_exit

  mov al, 0x00

get_chs_info_exit:
  ret







boot:
  cli
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7000

  cld

  ; store disk
  mov [disk], dl
  
  mov al, [disk]

  call lba_capabilities
  cmp al, 0x01

  jne chs_stage2_load_first_sector

  mov ah, [disk]
  mov al, 0x1 ; second sector
  mov si, 0x1 ; load only one sector
  mov di, 0x7e0; segment
  mov dx, 0x00; offset

  call lba_load_sector
  cmp al, 0x01

  jne lba_error
  
  jmp read_stage2_headers

chs_stage2_load_first_sector:  
  mov al, [disk]

  call get_chs_info
  cmp al, 0x01

  jne chs_config_error

  mov [prop_head], dh
  mov al, cl
  and cl, 0x3F
  mov [prop_sec], cl
  shl ax, 2
  mov al, ch
  mov [prop_cil], al  

  ; destination address for chs
  mov ax, 0x7e0
  mov es, ax
  mov bx, 0x00
  mov ah, 0x02
  mov al, 1 ; for the header at least
  mov ch, 0x0
  mov cl, 0x2
  mov dh, 0x0
  mov dl, [disk]
  int 0x13
   
  jc chs_error
  

read_stage2_headers:
  ; we loaded the first sector, we now have to check the headers contents
  mov ax, [0x7e00] ; first two bytes of magic
  cmp ax, 0x4813
  jne magic_error

  mov ax, [0x7e00 + 2] ; second two bytes of magic
  cmp ax, 0x7768
  jne magic_error

  ; now we have to load as many sectors as needed
  mov ax, [0x7e00 + 4]
  mov dx, [0x7e00 + 6]

  ; add to ax header length, which is 12
  add ax, 12
  adc dx, 0
  
  ; add 511  to get upper bound for n_sectors
  add ax, 511
  adc dx, 0

  mov cx, 9

shift_loop:
  shr dx, 1
  rcr ax, 1
  loop shift_loop
  
  mov [remaining_sectors_low], ax
  mov [remaining_sectors_high], dx

  sub word [remaining_sectors_low], 1
  sbb word [remaining_sectors_high], 0

  ; now we have number of sectors to load
  ; must decide between lba or chs again
  
  mov al, [disk]
  call lba_capabilities
  cmp al, 0x01

  ; if no capabilities switch to chs
  jne chs_load_stage2
  
  mov al, 'C'
  mov ah, 0x0e
  int 0x10
 
  mov ah, [disk]
  mov al, 0x2
  mov si, [remaining_sectors_low]
  mov di, 0x7e0
  mov dx, 0x0200
  
  cmp si, 0x00
  je load_stage2_loop_exit
 
  call lba_load_sector
  
  cmp al, 0x01

  jne lba_error  

  jmp load_stage2_loop_exit

chs_load_stage2:

  mov byte [cur_sec], 0x3
  mov byte [cur_cil], 0x0
  mov byte [cur_head], 0x0
  
  xor bx, bx

  mov ax, 0x7e0
  mov es, ax

chs_load_stage2_loop:
  mov ax, [remaining_sectors_high]
  or ax, [remaining_sectors_low]
  jz load_stage2_loop_exit ; if 0 we are done, exit
  
  add bx, 0x0200
  jnc no_wrap
  mov ax, es
  add ax, 0x1000
  mov es, ax

no_wrap:
  mov ah, 0x02
  mov al, 1
  mov ch, byte [cur_cil]
  mov cl, byte [cur_sec]
  mov dh, byte [cur_head]
  mov dl, byte [disk]
  int 0x13
  
  jc chs_error

  sub word [remaining_sectors_low], 1
  sbb word [remaining_sectors_high], 0

  add byte [cur_sec], 1
  mov al, [cur_sec]
  cmp byte al, [prop_sec] 
  jne chs_load_stage2_loop
  
  mov byte [cur_sec], 0x01
  add byte [cur_head], 0x01
  mov al, [cur_head]
  cmp byte al, [prop_head]
  jne chs_load_stage2_loop

  mov byte [cur_head], 0x00
  add byte [cur_cil], 1

  jmp chs_load_stage2_loop



load_stage2_loop_exit:
  mov ax, [0x7e00+8]
  mov word [jump_target], ax
  mov word [jump_target+2], 0x7e0
  mov dl, [disk]
  jmp far [jump_target]

magic_error:
  jmp booterror

lba_error:
  jmp booterror

chs_error:
  jmp booterror

chs_config_error:
  jmp chs_error

booterror:
  mov al, 'E'
  mov ah, 0x0e
  int 0x10
  hlt
  jmp booterror


;times 446 - ($-$$) db 0

;db 0x80
;db 0x00,0x01,0x00
;db 0x0C
;db 0x00,0x01,0x00
;dd 0x000000001
;dd 0x000001000

times 510 - ($-$$) db 0
dw 0xAA55
