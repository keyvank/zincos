#pragma once

#include "cpu/types.h"
#include "cpu/isr.h"
#include "kernel/memory.h"

enum PAGE_TABLE_ENTRY_FLAGS {
	PAGE_TABLE_ENTRY_PRESENT = 1,
	PAGE_TABLE_ENTRY_WRITABLE = 2,
	PAGE_TABLE_ENTRY_USER = 4,
	PAGE_TABLE_ENTRY_WRITETHOUGH = 8,
	PAGE_TABLE_ENTRY_NOT_CACHEABLE = 0x10,
	PAGE_TABLE_ENTRY_ACCESSED = 0x20,
	PAGE_TABLE_ENTRY_DIRTY = 0x40,
	PAGE_TABLE_ENTRY_PAT = 0x80,
	PAGE_TABLE_ENTRY_CPU_GLOBAL = 0x100,
	PAGE_TABLE_ENTRY_LV4_GLOBAL = 0x200,
  PAGE_TABLE_ENTRY_FRAME = 0x7FFFF000
};

enum PAGE_DIRECTORY_ENTRY_FLAGS {
	PAGE_DIRECTORY_ENTRY_PRESENT = 1,
	PAGE_DIRECTORY_ENTRY_WRITABLE = 2,
	PAGE_DIRECTORY_ENTRY_USER = 4,
	PAGE_DIRECTORY_ENTRY_PWT = 8,
	PAGE_DIRECTORY_ENTRY_PCD = 0x10,
	PAGE_DIRECTORY_ENTRY_ACCESSED = 0x20,
	PAGE_DIRECTORY_ENTRY_DIRTY = 0x40,
	PAGE_DIRECTORY_ENTRY_4MB = 0x80,
	PAGE_DIRECTORY_ENTRY_CPU_GLOBAL = 0x100,
	PAGE_DIRECTORY_ENTRY_LV4_GLOBAL = 0x200,
  PAGE_DIRECTORY_ENTRY_FRAME = 0x7FFFF000
};

typedef u32_t page_table_entry_t;

typedef struct {
	page_table_entry_t entries[1024];
} __attribute__((packed)) page_table_t;

typedef struct {
	page_table_entry_t entries[1024];
} __attribute__((packed)) page_directory_t;

void init_paging(isr_t const p_page_fault_handler);

page_table_t *map_physical(page_directory_t *p_directory, page_directory_t *p_template, u32_t p_virtual, u32_t p_physical, u32_t p_flags, memory &p_memory);
