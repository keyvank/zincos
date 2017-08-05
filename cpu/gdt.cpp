#include "cpu/gdt.h"
#include "cpu/asmutil.h"
#include "kernel/util.h"

#define GDT_NUM_ENTRIES (6)

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

tss_entry_t tss_entry;
void tss_install(u32_t const p_index, u32_t p_ss0, u32_t p_esp0) {
	u32_t base = (u32_t) &tss_entry;
  u32_t limit = base + sizeof(tss_entry_t);
  gdt_set_gate(p_index, base, limit, 0xE9, 0x00);
  memory_set((u8_t *)&tss_entry, 0, sizeof(tss_entry_t));
  tss_entry.ss0  = p_ss0;
  tss_entry.esp0 = p_esp0;
  tss_entry.cs   = KERNEL_CODE_SEGMENT | 0x03;
  tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = (KERNEL_DATA_SEGMENT | 0x03);
}

void gdt_install() {
	gdt_reg.limit = (sizeof(gdt_entry_t) * GDT_NUM_ENTRIES) - 1;
	gdt_reg.base = (u32_t)&gdt;
	gdt_set_gate(0, 0, 0, 0, 0); /* Null descriptor */
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* Kernel Code Segment. Base: 0, Limit: 4GB */
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* Kernel Data Segment. Base: 0, Limit: 4GB */
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* User Code Segment. Base: 0, Limit: 4GB */
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* User Data Segment. Base: 0, Limit: 4GB */
	tss_install(5, KERNEL_DATA_SEGMENT, 0x00);
  gdt_load(&gdt_reg, KERNEL_CODE_SEGMENT, KERNEL_DATA_SEGMENT);
	tss_load(TSS_SEGMENT | 0x03);
}
