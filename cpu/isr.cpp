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
}

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_irq_install() {
    set_idt_gate(0, (u32_t)isr0);
    set_idt_gate(1, (u32_t)isr1);
    set_idt_gate(2, (u32_t)isr2);
    set_idt_gate(3, (u32_t)isr3);
    set_idt_gate(4, (u32_t)isr4);
    set_idt_gate(5, (u32_t)isr5);
    set_idt_gate(6, (u32_t)isr6);
    set_idt_gate(7, (u32_t)isr7);
    set_idt_gate(8, (u32_t)isr8);
    set_idt_gate(9, (u32_t)isr9);
    set_idt_gate(10, (u32_t)isr10);
    set_idt_gate(11, (u32_t)isr11);
    set_idt_gate(12, (u32_t)isr12);
    set_idt_gate(13, (u32_t)isr13);
    set_idt_gate(14, (u32_t)isr14);
    set_idt_gate(15, (u32_t)isr15);
    set_idt_gate(16, (u32_t)isr16);
    set_idt_gate(17, (u32_t)isr17);
    set_idt_gate(18, (u32_t)isr18);
    set_idt_gate(19, (u32_t)isr19);
    set_idt_gate(20, (u32_t)isr20);
    set_idt_gate(21, (u32_t)isr21);
    set_idt_gate(22, (u32_t)isr22);
    set_idt_gate(23, (u32_t)isr23);
    set_idt_gate(24, (u32_t)isr24);
    set_idt_gate(25, (u32_t)isr25);
    set_idt_gate(26, (u32_t)isr26);
    set_idt_gate(27, (u32_t)isr27);
    set_idt_gate(28, (u32_t)isr28);
    set_idt_gate(29, (u32_t)isr29);
    set_idt_gate(30, (u32_t)isr30);
    set_idt_gate(31, (u32_t)isr31);

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
    set_idt_gate(32, (u32_t)irq0);
    set_idt_gate(33, (u32_t)irq1);
    set_idt_gate(34, (u32_t)irq2);
    set_idt_gate(35, (u32_t)irq3);
    set_idt_gate(36, (u32_t)irq4);
    set_idt_gate(37, (u32_t)irq5);
    set_idt_gate(38, (u32_t)irq6);
    set_idt_gate(39, (u32_t)irq7);
    set_idt_gate(40, (u32_t)irq8);
    set_idt_gate(41, (u32_t)irq9);
    set_idt_gate(42, (u32_t)irq10);
    set_idt_gate(43, (u32_t)irq11);
    set_idt_gate(44, (u32_t)irq12);
    set_idt_gate(45, (u32_t)irq13);
    set_idt_gate(46, (u32_t)irq14);
    set_idt_gate(47, (u32_t)irq15);

    set_idt(); // Load with ASM
}

/* To print the message which defines every exception */
char const *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
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
    kprint("Received interrupt: ");
    char_t s[3];
    int_to_ascii(p_registers.int_no, s);
    kprint(s);
    kprint("\n");
    kprint(exception_messages[p_registers.int_no]);
    kprint("\n");
}

void register_interrupt_handler(u8_t const p_index, isr_t const p_handler) {
    interrupt_handlers[p_index] = p_handler;
}

extern "C" void irq_handler(registers_t const p_registers) {
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (p_registers.int_no >= 40) port_byte_out(0xA0, 0x20); /* Slave */
    port_byte_out(0x20, 0x20); /* Master */
    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[p_registers.int_no] != 0) {
        isr_t handler = interrupt_handlers[p_registers.int_no];
        handler(p_registers);
    }
}
