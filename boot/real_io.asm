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

; Reads AX sectors of drive DL from LBA CX to [BX] and jumps to READ_DONE on done
load_disk_real:
	mov word [.SIZE], ax
	mov di, 0
	mov si, .DAPACK		; address of "disk address packet"
	mov word [si+4],bx
	mov dword [si+8],0
	add word [si+8],cx
.lp:
	mov ah, 0x42
	int 0x13
	mov si, .DAPACK		; address of "disk address packet"
	jc short .error
	add word [si+4],512
	add dword [si+8],1
	inc di

	cmp di,[.SIZE]
	jne .lp
	jmp READ_DONE
.error:
	mov bx, .MSG_READ_ERROR
  call print_real
	jmp $
.DAPACK:
	db 0x10
	db 0
	dw 1 ; INT 0x13 resets this to # of blocks actually read/written
	dw 0 ; Memory buffer destination address
	dw 0
	dd 0 ; Put the LBA to read in this spot
	dd 0 ; More storage bytes only for big LBAs ( > 4 bytes )
.SIZE: dw 0
.MSG_READ_ERROR: db 'Error reading disk.',0


; Load number of heads in DH and number of sectors per track in CL from drive DL
load_disk_geometry_real:
	push ax
	push bx

	xor ax, ax
	mov di, ax
	mov es, ax
	mov ah, 8
	int 0x13
	inc dh

	pop bx
	pop ax
	ret
