; Bootsector is loaded at 0x7c00 in memory
[ORG 0x7c00]
[BITS 16]

KERNEL_OFFSET equ 0x1000 ; The same one we used when linking the kernel

; Clear screen.
call clear_real

; Setting up stack
mov bp, 0x9000
mov sp, bp

; Write that we are in Real-mode
mov bx, MSG_REAL_MODE
call print_real
call print_newline_real

; Load kernel
mov bx, MSG_LOAD_KERNEL
call print_real
call print_newline_real
mov bx, KERNEL_OFFSET ; Read from disk and store in KERNEL_OFFSET
mov al, 16 ; Read four sectors
mov dl, [BOOT_DRIVE]
mov cl, 0x02 ; From second sector (First one is boot sector)
mov ch, 0 ; Cylinder 0
mov dh, 0 ; Head 0
call load_disk_real

call switch_to_pm
; Infinite loop, Never going to execute
jmp $

; Include files
%include "./boot/gdt.asm"
%include "./boot/real_io.asm"
%include "./boot/protected_io.asm"

[BITS 32]
BEGIN_PROTECTED:
	mov ebx, MSG_PROTECTED_MODE
	call print_protected
	call KERNEL_OFFSET
	jmp $ ; Wait here when kernel returned control to us!

; Constants
NEW_LINE: db 0x0a,0x0d,0
MSG_REAL_MODE: db "Hello ZincOS, We are in Real-Mode!",0
MSG_PROTECTED_MODE: db "Yoooohoooooo! We are in Protected-mode!",0
MSG_LOAD_KERNEL: db "Loading kernel into memory...",0

BOOT_DRIVE: db 0x80 ; It is a good idea to store it in memory because DL may get overwritten

; MBR signature
times 510-($-$$) db 0
dw 0xAA55
