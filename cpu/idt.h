#pragma once

#include "cpu/types.h"

void set_idt_gate(size_t const p_index, u32_t const p_handler, bool const p_is_usermode);
void set_idt();
