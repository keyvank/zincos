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

global enter_usermode
enter_usermode:
  push ebp
  mov ebp, esp
  cli
  mov ax, 0x23 ; Usermode Data Selector is 0x20 (GDT entry 3). Also sets RPL to 3
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  push 0x23
  mov eax,esp
  push eax
  pushfd
  pop eax
  or eax,0x200
  push eax
  push 0x1b
  lea eax, [.temp]
	push eax
	iretd
.temp:
  add esp, 4
  leave
  ret

global tss_load
tss_load:
  push ebp
  mov ebp, esp
  mov ax, word [ebp + 8]
  ltr ax
  leave
  ret

global get_esp
get_esp:
  push ebp
  mov ebp, esp
  lea eax, [esp + 8]
  leave
  ret

global get_eip
get_eip:
  pop eax
  jmp eax
