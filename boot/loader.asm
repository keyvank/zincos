[ORG 0x1000] ; Loader is from 0x1000 to 0x15ff, from 0x1600 starts the kernel
[BITS 16]

call enable_a20 ; (Defined in a20.asm)

mov ax, KERNEL_SIZE
mov bx, KERNEL_ADDR
mov cx, KERNEL_LBA
mov dl, BOOT_DRIVE
jmp load_disk_real
READ_DONE:

sti

; Set memory size in multibit_info structure
xor		eax, eax
xor		ebx, ebx
call	get_mem_size_64mb
mov		word [boot_info+multiboot_info.memory_high], bx
mov		word [boot_info+multiboot_info.memory_low], ax

; Set memory map address in multibit_info structure
xor		eax, eax
mov		ds, ax
mov		di, memory_map
call	get_mem_map
mov		word [boot_info+multiboot_info.mmap_addr], di

call switch_to_pm ; (Defined in protected_mode.asm)

; Infinite loop, Never going to execute
jmp $

; After you call switch_to_pm, CPU will jump to BEGIN_PROTECTED after enabling Protected-Mode
[BITS 32]
BEGIN_PROTECTED:

	mov ebp, 0x90000 ; Update the stack right at the top of the free space
	mov esp, ebp

	; Say that we are in Protected-mode, this time using with the Protected-mode version of our print function!
	mov ebx, MSG_PROTECTED_MODE
	call print_protected ; (Defined in protected_io.asm)

	; Jump to kernel!
	mov eax, 0x2BADB002		; multiboot specs say eax should be this
	mov ebx, 0
	;mov edx, [ImageSize]

	push dword boot_info
	jmp KERNEL_ADDR

	cli
	hlt

	jmp $ ; Wait here when kernel returned control to us!

%include "./boot/protected_mode.asm"
%include "./boot/a20.asm"
%include "./boot/gdt.asm"
%include "./boot/memory.asm"
%include "./boot/real_io.asm"
%include "./boot/protected_io.asm"
%include "./boot/multiboot.asm"

MSG_PROTECTED_MODE: db "Protected-mode!",0
LOADER_ADDR equ 0x1000
KERNEL_ADDR equ LOADER_ADDR + 0x600 ; The same one we used when linking the kernel
KERNEL_SIZE equ 0x70 ; Sectors
KERNEL_LBA equ 4 ; At LBA 0 is bootsector and at LBA 1 starts loader and at LBA 4 starts kernel
BOOT_DRIVE equ 0x80

memory_map: ; Free space for memory map

times 1536-($-$$) db 0 ; Loader is 1536 (0x600) bytes long
