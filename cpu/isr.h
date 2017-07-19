#ifndef __CPU__ISR__
#define __CPU__ISR__

#include "cpu/types.h"

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
} __attribute__((packed)) registers_t;
typedef void (*isr_t)(registers_t);

void register_interrupt_handler(u8_t const p_index, isr_t const p_handler);
void isr_irq_install();

#endif
