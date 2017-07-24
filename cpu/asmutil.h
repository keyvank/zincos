#pragma once

#include "cpu/types.h"

extern "C" void gdt_load(addr_t const p_gdt_reg_addr, u16_t const p_curr_code_seg, u16_t const p_curr_data_seg);
extern "C" void enable_paging();
extern "C" void load_page_directory(u32_t const * const p_page_directory);
