#include "cpu/idt.h"
#include "kernel/util.h"

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(size_t p_index, u32_t p_handler) {
    idt[p_index].low_offset = low_16(p_handler);
    idt[p_index].sel = KERNEL_CS;
    idt[p_index].always0 = 0;
    idt[p_index].flags = 0x8E;
    idt[p_index].high_offset = high_16(p_handler);
}

void set_idt() {
    idt_reg.base = (u32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    /* Don't make the mistake of loading &idt -- always load &idt_reg */
    __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_reg));
}
