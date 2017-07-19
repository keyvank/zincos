; This assembly file will be compiled to a 512-byte binary file and it will be the first sector of our disk (Each sector is 512-bytes wide)
; BIOS will load this sector at 0x7C00 and jump to it!
[ORG 0x7c00] ; Bootsector is loaded at 0x7c00 in memory
[BITS 16] ; We are currently in 16-bit Real-mode!

call clear_real ; Clear screen. (Defined in real_io.asm)

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

; Read from disk and store in KERNEL_OFFSET
mov bx, LOADER_ADDR ; Where to load the loader and kernel
mov al, LOADER_KERNEL_SIZE
mov dl, [BOOT_DRIVE]
mov cl, 0x02 ; Start reading from which sector? From the second sector (Kernel is placed after the bootsector. So it starts from second sector)
mov ch, 0 ; Cylinder 0
mov dh, 0 ; Head 0
call load_disk_real ; (Defined in real_io.asm)

call LOADER_ADDR

; Infinite loop, Never going to execute
jmp $

; Include files
%include "./boot/real_io.asm"

; Constants
NEW_LINE: db 0x0a,0x0d,0
MSG_REAL_MODE: db "Real-Mode!",0
MSG_LOAD_LOADER: db "Loading loader...",0

BOOT_DRIVE: db 0x80 ; It is a good idea to store it in memory because DL may get overwritten
LOADER_ADDR equ 0x1000 ; Second stage bootloader address
LOADER_KERNEL_SIZE equ 0x20 + 0x3 ; Sectors (0x20 sectors for kernel and 3 sectors for loader)

; MBR signature
times 510-($-$$) db 0
dw 0xAA55
