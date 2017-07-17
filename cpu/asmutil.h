#ifndef __KERNEL__ASMUTIL__
#define __KERNEL__ASMUTIL__

#include "cpu/types.h"

void gdt_load(addr_t const p_gdt_reg_addr, u16_t const p_curr_code_seg, u16_t const p_curr_data_seg);

#endif
