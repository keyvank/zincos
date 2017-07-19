#include "cpu/gdt.h"
#include "cpu/asmutil.h"
#include "kernel/util.h"

#define GDT_NUM_ENTRIES (5)

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

gdt_entry_t gdt[GDT_NUM_ENTRIES];
gdt_ptr_t gdt_reg;

static void gdt_set_gate(size_t const p_index, u32_t const p_base, u32_t const p_limit, u8_t const p_access, u8_t const p_gran) {
	gdt[p_index].base_low = (p_base & 0xFFFF);
	gdt[p_index].base_middle = (p_base >> 16) & 0xFF;
	gdt[p_index].base_high = (p_base >> 24) & 0xFF;
	gdt[p_index].limit_low = (p_limit & 0xFFFF);
	gdt[p_index].granularity = ((p_limit >> 16) & 0x0F);
	gdt[p_index].granularity |= (p_gran & 0xF0);
	gdt[p_index].access = p_access;
}

void gdt_install() {
	gdt_reg.limit = (sizeof(gdt_entry_t) * GDT_NUM_ENTRIES) - 1;
	gdt_reg.base = (u32_t)&gdt;
	gdt_set_gate(0, 0, 0, 0, 0); /* Null descriptor */
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* Kernel Code Segment. Base: 0, Limit: 4GB */
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* Kernel Data Segment. Base: 0, Limit: 4GB */
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* User Code Segment. Base: 0, Limit: 4GB */
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* User Data Segment. Base: 0, Limit: 4GB */
  gdt_load(&gdt_reg, KERNEL_CODE_SEGMENT, KERNEL_DATA_SEGMENT);
}
