[BITS 32] ; Using 32-bit protected mode

; Prints an ASCII text starting at EBX in Protected-mode
print_protected:
    pusha
    mov edx, .VIDEO_MEMORY
.loop:
    mov al, [ebx]
    mov ah, .WHITE_ON_BLACK

    cmp al, 0
    je .done

    mov [edx], ax
    add ebx, 1
    add edx, 2 ; Next video memory position (Each character has two bytes, one for the character number and one for color.

    jmp .loop

.done:
    popa
    ret
    .VIDEO_MEMORY equ 0xb8000
	.WHITE_ON_BLACK equ 0x0f

