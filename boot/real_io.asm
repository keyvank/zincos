[BITS 16] ; Real-mode functions

; Prints an ASCII text starting at BX in Real-mode.
print_real:
	push ax
	push si
	mov si,0
.loop:
	cmp	byte [bx+si],0
	je .done
	mov ah,0eH
	mov al,byte [bx+si]
	int 10H
	inc si
	jmp .loop
.done:
	pop si
	pop ax
	ret



; Prints new-line character.
print_newline_real:
	push bx
	mov bx, .NEW_LINE
	call print_real
	pop bx
	ret
	.NEW_LINE: db 0x0a,0x0d,0



; Clears screen.
clear_real:
	mov ah,06H
	mov bh,07H
	mov cx,0
	mov dh,24
	mov dl,79
	int 10H
	mov ah,02H
	mov bh,00H
	mov dx,0
	int 10H
	ret



; Print the value in DX in hexadecimal form 0x0000.
print_hex_real:
    pusha
    mov cx, 0 ; Index variable

.loop:
    cmp cx, 4 ; loop 4 times
    je .end

    ; Step 1. convert last char of 'dx' to ascii
    mov ax, dx ; We will use 'ax' as our working register
    and ax, 0x000f ; 0xXXXX -> 0x000X by masking first three to zeros
    add al, 0x30 ; Add 0x30 to N to convert it to ASCII
    cmp al, 0x39 ; If > 9, add extra 8 to represent 'A' to 'F'
    jle .step2
    add al, 7

.step2:
    ; Step 2. Get the correct position of the string to place our ASCII char
    ; bx <- Base address + String length - Index of char
    mov bx, .HEX_OUT + 5 ; Base + Length
    sub bx, cx  ; Our index variable
    mov [bx], al ; Copy the ASCII char on AL to the position pointed by BX
    ror dx, 4 ; 0x1234 -> 0x4123 -> 0x3412 -> 0x2341 -> 0x1234

    add cx, 1
    jmp .loop

.end:
    mov bx, .HEX_OUT
    call print_real
    popa
    ret

	.HEX_OUT: db '0x0000',0 ; reserve memory for our new string



; Load AL sectors from drive DL, sector CL, cylinder CH, head DH, into ES:BX in Real-mode.
load_disk_real:
	pusha
	push ax ; Save AX as it is going to be changed by BIOS
	mov ah, 0x02 ; Reading mode
	int 0x13
	jc .disk_error ; if error (stored in the carry bit)
	pop dx ; BIOS sets AL to the number of sectors read. Now DL has the previous AL value
	cmp al, dl ; These should be equal
	jne .sectors_error
	popa
	ret
	.DISK_ERROR: db "Disk Error. Code: ", 0
	.SECTORS_ERROR: db "Read error.", 0

.disk_error:
	mov bx, .DISK_ERROR
	call print_real
	mov dh, ah ; AH = Error code, DL = Disk drive that dropped the error
	call print_hex_real ; Check out the code at http://stanislavs.org/helppc/int_13-1.html
	jmp .disk_loop

.sectors_error:
	mov bx, .SECTORS_ERROR
	call print_real

.disk_loop:
	jmp $
