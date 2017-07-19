struc multiboot_info
	.flags resd 1 ; Required
	.memory_low resd 1 ; Memory size. Present if flags[0] is set
	.memory_high resd 1
	.boot_device resd 1 ; Boot device. Present if flags[1] is set
	.cmd_line resd 1 ; Kernel command line. Present if flags[2] is set
	.mods_count resd 1 ; Number of modules loaded along with kernel. Present if flags[3] is set
	.mods_addr resd 1
	.syms0 resd 1 ; Symbol table info. Present if flags[4] or flags[5] is set
	.syms1 resd 1
	.syms2 resd 1
	.mmap_length resd 1 ; Memory map. Present if flags[6] is set
	.mmap_addr resd 1
	.drives_length resd 1 ; Physical address of first drive structure. Present if flags[7] is set
	.drives_addr resd 1
	.config_table resd 1 ; ROM configuation table. Present if flags[8] is set
	.bootloader_name resd 1 ; Bootloader name. Present if flags[9] is set
	.apm_table resd 1 ; Advanced Power Management (APM) table. Present if flags[10] is set
	.vbe_control_info resd 1 ; Video Bios Extension (VBE). Present if flags[11] is set
	.vbe_mode_info resd 1
	.vbe_mode resw 1
	.vbe_interface_seg resw 1
	.vbe_interface_off resw 1
	.vbe_interface_len resw 1
endstruc

boot_info:
istruc multiboot_info
	at multiboot_info.flags, dd 0
	at multiboot_info.memory_low, dd 0
	at multiboot_info.memory_high, dd 0
	at multiboot_info.boot_device, dd 0
	at multiboot_info.cmd_line, dd 0
	at multiboot_info.mods_count, dd 0
	at multiboot_info.mods_addr, dd 0
	at multiboot_info.syms0, dd 0
	at multiboot_info.syms1, dd 0
	at multiboot_info.syms2, dd 0
	at multiboot_info.mmap_length, dd 0
	at multiboot_info.mmap_addr, dd 0
	at multiboot_info.drives_length, dd 0
	at multiboot_info.drives_addr, dd 0
	at multiboot_info.config_table, dd 0
	at multiboot_info.bootloader_name, dd 0
	at multiboot_info.apm_table, dd 0
	at multiboot_info.vbe_control_info, dd 0
	at multiboot_info.vbe_mode_info, dw 0
	at multiboot_info.vbe_interface_seg, dw 0
	at multiboot_info.vbe_interface_off, dw 0
	at multiboot_info.vbe_interface_len, dw 0
iend
