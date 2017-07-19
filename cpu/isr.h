#ifndef __CPU__ISR__
#define __CPU__ISR__

#include "cpu/types.h"

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

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

/* Struct which aggregates many registers */
typedef struct {
   u32_t ds; /* Data segment selector */
   u32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
   u32_t int_no, err_code; /* Interrupt number and error code (if applicable) */
   u32_t eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} registers_t;

typedef void (*isr_t)(registers_t);

void register_interrupt_handler(u8_t n, isr_t handler);
void isr_irq_install();
extern "C" void isr_handler(registers_t r);
extern "C" void irq_handler(registers_t r);

#endif
