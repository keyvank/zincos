#include "cpu/isr.h"
#include "cpu/idt.h"
#include "cpu/ports.h"
#include "drivers/vga.h"
#include "kernel/util.h"

isr_t interrupt_handlers[256];

/* ISRs reserved for CPU exceptions */
extern "C" {
  void isr0();
  void isr1();
  void isr2();
  void isr3();
  void isr4();
  void isr5();
  void isr6();
  void isr7();
  void isr8();
  void isr9();
  void isr10();
  void isr11();
  void isr12();
  void isr13();
  void isr14();
  void isr15();
  void isr16();
  void isr17();
  void isr18();
  void isr19();
  void isr20();
  void isr21();
  void isr22();
  void isr23();
  void isr24();
  void isr25();
  void isr26();
  void isr27();
  void isr28();
  void isr29();
  void isr30();
  void isr31();

  /* IRQ definitions */
  void irq0();
  void irq1();
  void irq2();
  void irq3();
  void irq4();
  void irq5();
  void irq6();
  void irq7();
  void irq8();
  void irq9();
  void irq10();
  void irq11();
  void irq12();
  void irq13();
  void irq14();
  void irq15();

  void isr123(); // For Syscalls
}

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_irq_install() {
    set_idt_gate(0, (u32_t)isr0, false);
    set_idt_gate(1, (u32_t)isr1, false);
    set_idt_gate(2, (u32_t)isr2, false);
    set_idt_gate(3, (u32_t)isr3, false);
    set_idt_gate(4, (u32_t)isr4, false);
    set_idt_gate(5, (u32_t)isr5, false);
    set_idt_gate(6, (u32_t)isr6, false);
    set_idt_gate(7, (u32_t)isr7, false);
    set_idt_gate(8, (u32_t)isr8, false);
    set_idt_gate(9, (u32_t)isr9, false);
    set_idt_gate(10, (u32_t)isr10, false);
    set_idt_gate(11, (u32_t)isr11, false);
    set_idt_gate(12, (u32_t)isr12, false);
    set_idt_gate(13, (u32_t)isr13, false);
    set_idt_gate(14, (u32_t)isr14, false);
    set_idt_gate(15, (u32_t)isr15, false);
    set_idt_gate(16, (u32_t)isr16, false);
    set_idt_gate(17, (u32_t)isr17, false);
    set_idt_gate(18, (u32_t)isr18, false);
    set_idt_gate(19, (u32_t)isr19, false);
    set_idt_gate(20, (u32_t)isr20, false);
    set_idt_gate(21, (u32_t)isr21, false);
    set_idt_gate(22, (u32_t)isr22, false);
    set_idt_gate(23, (u32_t)isr23, false);
    set_idt_gate(24, (u32_t)isr24, false);
    set_idt_gate(25, (u32_t)isr25, false);
    set_idt_gate(26, (u32_t)isr26, false);
    set_idt_gate(27, (u32_t)isr27, false);
    set_idt_gate(28, (u32_t)isr28, false);
    set_idt_gate(29, (u32_t)isr29, false);
    set_idt_gate(30, (u32_t)isr30, false);
    set_idt_gate(31, (u32_t)isr31, false);
    set_idt_gate(123, (u32_t)isr123, true);

    // Remap the PIC
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);

    // Install the IRQs
    set_idt_gate(32, (u32_t)irq0, false);
    set_idt_gate(33, (u32_t)irq1, false);
    set_idt_gate(34, (u32_t)irq2, false);
    set_idt_gate(35, (u32_t)irq3, false);
    set_idt_gate(36, (u32_t)irq4, false);
    set_idt_gate(37, (u32_t)irq5, false);
    set_idt_gate(38, (u32_t)irq6, false);
    set_idt_gate(39, (u32_t)irq7, false);
    set_idt_gate(40, (u32_t)irq8, false);
    set_idt_gate(41, (u32_t)irq9, false);
    set_idt_gate(42, (u32_t)irq10, false);
    set_idt_gate(43, (u32_t)irq11, false);
    set_idt_gate(44, (u32_t)irq12, false);
    set_idt_gate(45, (u32_t)irq13, false);
    set_idt_gate(46, (u32_t)irq14, false);
    set_idt_gate(47, (u32_t)irq15, false);

    set_idt(); // Load with ASM
}

/* To print the message which defines every exception */
char const *exception_messages[] = {
    "Division By Zero", // 0
    "Debug", // 1
    "Non Maskable Interrupt", // 2
    "Breakpoint", // 3
    "Into Detected Overflow", // 4
    "Out of Bounds", // 5
    "Invalid Opcode", // 6
    "No Coprocessor", // 7
    "Double Fault", // 8
    "Coprocessor Segment Overrun", // 9
    "Bad TSS", // 10
    "Segment Not Present", // 11
    "Stack Fault", // 12
    "General Protection Fault", // 13
    "Page Fault", // 14
    "Unknown Interrupt", // 15
    "Coprocessor Fault", // 16
    "Alignment Check", // 17
    "Machine Check", // 18
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

extern "C" void isr_handler(registers_t const p_registers) {
    if (interrupt_handlers[p_registers.int_no] != 0) {
        isr_t handler = interrupt_handlers[p_registers.int_no];
        handler(p_registers);
    }
}

void register_interrupt_handler(u8_t const p_index, isr_t const p_handler) {
    interrupt_handlers[p_index] = p_handler;
}

extern "C" void irq_handler(registers_t const p_registers) {
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (p_registers.int_no >= 40) port_byte_out(0xA0, 0x20); /* Slave */
    port_byte_out(0x20, 0x20); /* Master */
    if (interrupt_handlers[p_registers.int_no] != 0) {
        isr_t handler = interrupt_handlers[p_registers.int_no];
        handler(p_registers);
    }
}
