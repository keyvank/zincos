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

; Loading the kernel
mov bx, MSG_LOAD_KERNEL
call print_real
call print_newline_real

; Read from disk and store in KERNEL_OFFSET
mov bx, [KERNEL_OFFSET] ; Where to load the kernel
mov al, [KERNEL_SIZE]
mov dl, [BOOT_DRIVE]
mov cl, 0x02 ; Start reading from which sector? From the second sector (Kernel is placed after the bootsector. So it starts from second sector)
mov ch, 0 ; Cylinder 0
mov dh, 0 ; Head 0
call load_disk_real ; (Defined in real_io.asm)

call switch_to_pm ; (Defined in protected_mode.asm)
; Infinite loop, Never going to execute
jmp $

; Include files
%include "./boot/protected_mode.asm"
%include "./boot/gdt.asm"
%include "./boot/real_io.asm"
%include "./boot/protected_io.asm"

; After you call switch_to_pm, CPU will jump to BEGIN_PROTECTED after enabling Protected-Mode
[BITS 32]
BEGIN_PROTECTED:

	mov ebp, 0x90000 ; Update the stack right at the top of the free space
	mov esp, ebp

	; Say that we are in Protected-mode, this time using with the Protected-mode version of our print function!
	mov ebx, MSG_PROTECTED_MODE
	call print_protected ; (Defined in protected_io.asm)

	; Jump to kernel!
	mov ax, [KERNEL_OFFSET]
	call ax

	jmp $ ; Wait here when kernel returned control to us!

; Constants
NEW_LINE: db 0x0a,0x0d,0
MSG_REAL_MODE: db "We are in Real-Mode!",0
MSG_PROTECTED_MODE: db "Yoooohoooooo! We are in Protected-mode!",0
MSG_LOAD_KERNEL: db "Loading kernel into memory...",0

BOOT_DRIVE: db 0x80 ; It is a good idea to store it in memory because DL may get overwritten
KERNEL_OFFSET: dw 0x1000 ; The same one we used when linking the kernel
KERNEL_SIZE: db 0x20 ; Sectors

; MBR signature
times 510-($-$$) db 0
dw 0xAA55
