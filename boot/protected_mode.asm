[BITS 16] ; We are currently in Real-mode

; Switching to Protected-Mode.
switch_to_pm:
  cli
  lgdt [gdt_descriptor] ; Load GDT Descriptor define in gdt.asm
  mov eax, cr0
  or eax, 0x1 ; Set 32-bit mode bit in cr0
  mov cr0, eax
  jmp CODE_SEG:.init_pm ; Far jump to init_pm, this will force the CS (CodeSegment register) to change to CODE_SEG. (Note that CODE_SEG is a GDT segment defined in gdt.asm)
[BITS 32] ; Now we are in Protected-mode
.init_pm: ; We are now using 32-bit instructions
; Update the segment registers
  mov ax, DATA_SEG
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

	jmp BEGIN_PROTECTED
