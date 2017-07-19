#include "cpu/idt.h"
#include "cpu/gdt.h"
#include "kernel/util.h"

typedef struct {
    u16_t low_offset; /* Lower 16 bits of handler function address */
    u16_t sel; /* Kernel segment selector */
    u8_t always0;
    /* First byte
     * Bit 7: "Interrupt is present"
     * Bits 6-5: Privilege level of caller (0=kernel..3=user)
     * Bit 4: Set to 0 for interrupt gates
     * Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
    u8_t flags;
    u16_t high_offset; /* Higher 16 bits of handler function address */
} __attribute__((packed)) idt_gate_t ;

typedef struct {
    u16_t limit;
    u32_t base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(size_t const p_index, u32_t const p_handler) {
    idt[p_index].low_offset = low_16(p_handler);
    idt[p_index].sel = KERNEL_CODE_SEGMENT;
    idt[p_index].always0 = 0;
    idt[p_index].flags = 0x8E;
    idt[p_index].high_offset = high_16(p_handler);
}

void set_idt() {
    idt_reg.base = (u32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_reg));
}
