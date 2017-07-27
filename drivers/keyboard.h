#pragma once

#include "cpu/isr.h"

void init_keyboard(isr_t const p_handler);
void print_letter(u8_t scancode);
