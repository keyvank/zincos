#ifndef __CPU__GDT__
#define __CPU__GDT__

#include "cpu/types.h"

typedef struct {
	u16_t limit_low;
	u16_t base_low;
	u8_t base_middle;
	u8_t access;
	u8_t granularity;
	u8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
  u16_t limit;
  u32_t base;
} __attribute__((packed)) gdt_ptr_t;

void gdt_install();

#endif
