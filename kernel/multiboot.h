#ifndef __KERNEL__BOOTINFO__
#define __KERNEL__BOOTINFO__

#include "cpu/types.h"

typedef struct {
	u32_t	flags;
	u32_t	memoryLo;
	u32_t	memoryHi;
	u32_t	bootDevice;
	u32_t	cmdLine;
	u32_t	modsCount;
	u32_t	modsAddr;
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
} multiboot_info_t;

#endif
