[BITS 32]

; Load GDT.
; 1st argument: GDT address
; 2nd argument: CS after lgdt
; 3nd argument: DS, ES, FS, GS, SS after lgdt
global gdt_load
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

global enable_paging
enable_paging:
  push ebp
  mov ebp, esp
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax
  leave
  ret

global load_page_directory
load_page_directory:
  push ebp
  mov ebp, esp
  mov eax, dword [ebp+8]
  mov cr3, eax
  leave
  ret
