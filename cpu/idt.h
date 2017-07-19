#ifndef __CPU__IDT__
#define __CPU__IDT__

#include "cpu/types.h"

void set_idt_gate(size_t const p_index, u32_t const p_handler);
void set_idt();

#endif
