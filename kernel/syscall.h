#pragma once

#include "cpu/types.h"
#include "cpu/isr.h"

void init_syscalls();
void set_syscall(size_t const p_index, void * const p_function);
