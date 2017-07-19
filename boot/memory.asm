[BITS 16]

;	Get memory map from BIOS and store it in ES:DI

get_mem_map:
	pushad
	xor ebx, ebx
	mov edx, 'PAMS' ; 'SMAP'
	mov eax, 0xe820
	mov ecx, 24 ; Memory map entries are 24 bytes
	int 0x15 ; Get first entry
	jc .error
	cmp eax, 'PAMS' ; BIOS should return SMAP in eax
	jne .error
	test ebx, ebx ; If EBX=0 then list is one entry long; bail out
	je .error
	jmp .start
.next_entry:
	mov edx, 'PAMS' ; Some BIOS's trash this register
	mov ecx, 24 ; Entry is 24 bytes
	mov eax, 0xe820
	int 0x15 ; Get next entry
.start:
	jcxz .skip_entry ; If actual returned bytes is 0, skip entry
.notext:
	mov ecx, [es:di + 8] ; Get length (low dword)
	test ecx, ecx ; If length is 0 skip it
	jne short .good_entry
	mov ecx, [es:di + 12] ; Get length (upper dword)
	jecxz .skip_entry ; If length is 0 skip it
.good_entry:
	add di, 24 ; Point DI to next entry in buffer
.skip_entry:
	cmp ebx, 0 ; If EBX return is 0, list is done
	jne .next_entry ; Get next entry
	jmp .done
.error:
	stc
.done:
	popad
	ret



;	Get memory size for >64M configuations
; Output values: AX = KB between 1MB and 16MB, BX = Number of 64K blocks above 16MB
; In case of error:	BX = 0 and AX = -1

get_mem_size_64mb:
	push ecx
	push edx
	xor ecx, ecx
	xor edx, edx
	mov ax, 0xe801
	int 0x15
	jc .error
	cmp ah, 0x86 ; Unsupported function
	je .error
	cmp ah, 0x80 ; Invalid command
	je .error
	jcxz .use_ax ; BIOS may have stored it in AX, BX or CX, DX. Test if CX is 0
	mov ax, cx ; Its not, so it should contain memory size; store it
	mov bx, dx

.use_ax:
	pop edx ; Mem size is in AX and BX already, Return it
	pop ecx
	ret

.error:
	mov ax, -1
	mov bx, 0
	pop edx
	pop ecx
	ret
