#pragma once

#include "cpu/types.h"

#define KERNEL_CODE_SEGMENT (0x08)
#define KERNEL_DATA_SEGMENT (0x10)
#define USER_CODE_SEGMENT (0x18)
#define USER_DATA_SEGMENT (0x20)
#define TSS_SEGMENT (0x28)

typedef struct {
	u32_t prev_tss;
	u32_t esp0;
	u32_t ss0;
	u32_t esp1;
	u32_t ss1;
	u32_t esp2;
	u32_t ss2;
	u32_t cr3;
	u32_t eip;
	u32_t eflags;
	u32_t eax;
	u32_t ecx;
	u32_t edx;
	u32_t ebx;
	u32_t esp;
	u32_t ebp;
	u32_t esi;
	u32_t edi;
	u32_t es;
	u32_t cs;
	u32_t ss;
	u32_t ds;
	u32_t fs;
	u32_t gs;
	u32_t ldt;
	u16_t trap;
	u16_t iomap;
} __attribute__((packed)) tss_entry_t;

void gdt_install();
void tss_set_kernel_stack(u32_t const p_stack);
