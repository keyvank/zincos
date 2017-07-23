#pragma once

#include "cpu/types.h"

enum PAGE_PTE_FLAGS {
	I86_PTE_PRESENT = 1,
	I86_PTE_WRITABLE = 2,
	I86_PTE_USER = 4,
	I86_PTE_WRITETHOUGH = 8,
	I86_PTE_NOT_CACHEABLE = 0x10,
	I86_PTE_ACCESSED = 0x20,
	I86_PTE_DIRTY = 0x40,
	I86_PTE_PAT = 0x80,
	I86_PTE_CPU_GLOBAL = 0x100,
	I86_PTE_LV4_GLOBAL = 0x200,
  I86_PTE_FRAME = 0x7FFFF000
};

enum PAGE_PDE_FLAGS {
	I86_PDE_PRESENT = 1,
	I86_PDE_WRITABLE = 2,
	I86_PDE_USER = 4,
	I86_PDE_PWT = 8,
	I86_PDE_PCD = 0x10,
	I86_PDE_ACCESSED = 0x20,
	I86_PDE_DIRTY = 0x40,
	I86_PDE_4MB = 0x80,
	I86_PDE_CPU_GLOBAL = 0x100,
	I86_PDE_LV4_GLOBAL = 0x200,
  I86_PDE_FRAME = 0x7FFFF000
};

typedef u32_t page_table_entry_t;

typedef struct {
	page_table_entry_t entries[1024];
} __attribute__((packed)) page_table_t;

typedef struct {
	page_table_entry_t entries[1024];
} __attribute__((packed)) page_directory_t;
