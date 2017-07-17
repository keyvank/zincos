[BITS 32]

global gdt_load     ; Allows the C code to link to this
gdt_load:
    push ebp
    mov ebp, esp
    pusha
    mov eax, dword [ebp+8]
    xor ebx, ebx
    mov bx, word [ebp+12]
    xor ecx, ecx
    mov cx, word [ebp+16]

    lgdt [eax]
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov gs, cx
    mov ss, cx

    push ebx
    push .far
    retf ; Far jump to .far

.far:
    popa
    leave
    ret
