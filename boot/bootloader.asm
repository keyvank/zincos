[ORG 0x7c00]				; Leave the Interrupt Vector Table

jmp	start				; Jump from variables!

; Variables declared here

msg:	db	"Welcome to Zinc OS Bootloader!",0

; End of variable declaration


start:					; We're gonna begin!
		mov		ah,06H
		mov		bh,07H
		mov		cx,0
		mov		dh,24
		mov		dl,79
		int		10H
		
		mov		ah,02H
		mov		bh,00H
		mov		dx,0
		int		10H
		
		call	print
		
		
		jmp		$		; Infinite loop



print:
		push	ax
		push	si
		mov		si,0
write:	cmp		byte [msg+si],0
		je		done
		mov		ah,0eH
		mov		al,byte [msg+si]
		int		10H
		inc		si
		jmp		write
done:	pop		si
		pop		ax
		ret

; MBR signature
times 510-($-$$) db 0
dw 0xAA55
