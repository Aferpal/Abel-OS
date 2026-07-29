org 0x7c00
start: jmp boot

disk db 0x0

remaining_sectors_high dw 0x00
remaining_sectors_low dw 0x00

cur_cil db 0x0
cur_sec db 0x0
cur_head db 0x0

dq 0x0

jump_target:
  dw 12 
  dw 0x07e0

boot:
  cli
  cld

  mov [disk], dl

  mov ax, 0x7e0
  mov es, ax
  xor bx, bx
 
  mov ah, 0x02
  mov al, 1 ; for the header at least
  mov ch, 0x0
  mov cl, 0x2
  mov dh, 0x0
  mov dl, [disk]
  int 0x13
   
  jc booterror
  
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

  add ax, 12
  adc dx, 0

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

  mov byte [cur_sec], 0x3
  mov byte [cur_cil], 0x0
  mov byte [cur_head], 0x0
  
  xor bx, bx

load_stage2_loop:
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
  
  jc booterror

  sub word [remaining_sectors_low], 1
  sbb word [remaining_sectors_high], 0

  add byte [cur_sec], 1
  cmp byte [cur_sec], 63
  jne load_stage2_loop
  
  mov byte [cur_sec], 0x01
  add byte [cur_head], 0x01
  cmp byte [cur_head], 15
  jne load_stage2_loop

  mov byte [cur_head], 0x00
  add byte [cur_cil], 1

  jmp load_stage2_loop



load_stage2_loop_exit:

  mov ax, [0x7e00+8]
  mov word [jump_target], ax
  mov word [jump_target+2], 0x7e0
  jmp far [jump_target]

magic_error:
  mov ah, 0x0e
  mov al, 'M'
  int 0x10
  jmp booterror

booterror:
  mov al, 'E'
  mov ah, 0x0e
  int 0x10
  hlt
  jmp booterror

times 510 - ($-$$) db 0
dw 0xAA55
