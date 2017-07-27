#pragma once

#include "kernel/util.h"
#include "cpu/isr.h"

void init_timer(u32_t p_frequency, isr_t const p_handler);
