; This assembly file will be compiled to a 512-byte binary file and it will be the first sector of our disk (Each sector is 512-bytes wide)
; BIOS will load this sector at 0x7C00 and jump to it!
[ORG 0x7c00] ; Bootsector is loaded at 0x7c00 in memory
[BITS 16] ; We are currently in 16-bit Real-mode!

call clear_real ; Clear screen. (Defined in real_io.asm)

; Zeroing segment registers
mov ax, 0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax
jmp 0:cs0
cs0:

; Setting up stack (In real mode!)
mov bp, 0x9000
mov sp, bp

; Write that we are in Real-mode (print_real and print_newline_real defined in real_io.asm)
mov bx, MSG_REAL_MODE
call print_real
call print_newline_real

; Loading the loader
mov bx, MSG_LOAD_LOADER
call print_real
call print_newline_real

mov ax, LOADER_SIZE
mov bx, LOADER_ADDR
mov cx, LOADER_LBA
mov dl, BOOT_DRIVE
jmp load_disk_real
READ_DONE:
  call LOADER_ADDR

; Infinite loop, Never going to execute
jmp $

; Include files
%include "./boot/real_io.asm"

; Constants
NEW_LINE: db 0x0a,0x0d,0
MSG_REAL_MODE: db "Real-Mode!",0
MSG_LOAD_LOADER: db "Loading loader...",0

BOOT_DRIVE equ 0x80 ; It is a good idea to store it in memory because DL may get overwritten
LOADER_ADDR equ 0x1000 ; Second stage bootloader address
LOADER_SIZE equ 0x3 ; Sectors (0x33 sectors for kernel and 3 sectors for loader)
LOADER_LBA equ 1 ; At LBA 0 is bootsector and at LBA 1 starts loader

; MBR signature
times 510-($-$$) db 0
dw 0xAA55
