#pragma once

#include "cpu/types.h"

typedef struct {
	u32_t	flags;
	u32_t	memory_low;
	u32_t	memory_high;
	u32_t	boot_device;
	u32_t	cmd_line;
	u32_t	mods_count;
	u32_t	mods_addr;
	u32_t	syms0;
	u32_t	syms1;
	u32_t	syms2;
	u32_t	mmap_length;
	u32_t	mmap_addr;
	u32_t	drives_length;
	u32_t	drives_addr;
	u32_t	config_table;
	u32_t	bootloader_name;
	u32_t	aptable;
	u32_t	vbe_control_info;
	u32_t	vbe_mode_info;
	u16_t	vbe_mode;
	u32_t	vbe_interface_addr;
	u16_t	vbe_interface_len;
} __attribute__((packed)) multiboot_info_t;

typedef struct {
	u32_t	start_low;
	u32_t	start_high;
	u32_t	size_low;
	u32_t	size_high;
	u32_t	type;
	u32_t	acpi_3_0;
} __attribute__((packed)) multiboot_memory_map_t;
