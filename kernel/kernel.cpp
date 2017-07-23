#include "kernel/kernel.h"
#include "drivers/vga.h"
#include "kernel/util.h"
#include "cpu/isr.h"
#include "cpu/idt.h"
#include "cpu/gdt.h"
#include "drivers/keyboard.h"
#include "kernel/multiboot.h"
#include "kernel/memory.h"

kernel::kernel(multiboot_info_t const &p_multiboot_info) {
  clear_screen();
	kprint("We are in the kernel!\nWelcome to ZincOS!\n\n");

	multiboot_memory_map_t *region=(multiboot_memory_map_t *)p_multiboot_info.mmap_addr;
	while(true) {
		if (region->type==1 && region->start_low > (64 * _KB)){
			memory_region reg{reinterpret_cast<addr_t>(region->start_low),region->size_low};
			memory m(reg);
			kprint(m.get_block_count());
			kprint(" blocks initialized!\n");
			break;
		}
		region++;
		if (region->start_low==0)
			break;
	}

	gdt_install();
  isr_irq_install();
	asm volatile("sti");
	init_keyboard();


	//init_timer(50);
}
