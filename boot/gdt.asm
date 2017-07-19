; Temporary GDT for going into protected mode and calling kernel.
; Kernel will reinitialize GDT.

; No need for specifying the BITS directive

gdt_start: ; The GDT starts with a null 8-byte
    dd 0x0 ; 4 byte
    dd 0x0 ; 4 byte

; GDT for Code Segment. base = 0x00000000, length = 0xfffff
; for flags, refer the documentation
gdt_code:
    dw 0xffff    ; Segment length, bits 0-15
    dw 0x0       ; Segment base, bits 0-15
    db 0x0       ; Segment base, bits 16-23
    db 10011010b ; Flags (8 bits)
    db 11001111b ; Flags (4 bits) + Segment length, bits 16-19
    db 0x0       ; Segment base, bits 24-31

; GDT for Data Segment. base and length identical to code segment
; Some flags changed
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

; GDT Descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size (16 bit), always one less of its true size
    dd gdt_start ; Address (32 bit)

; Define some constants for later use
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
