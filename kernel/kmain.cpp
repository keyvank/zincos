#include "drivers/vga.h"
#include "kernel/util.h"
#include "cpu/isr.h"
#include "cpu/idt.h"
#include "cpu/gdt.h"
#include "drivers/keyboard.h"
#include "kernel/multiboot.h"

typedef struct {
	u32_t	startLo;
	u32_t	startHi;
	u32_t	sizeLo;
	u32_t	sizeHi;
	u32_t	type;
	u32_t	acpi_3_0;
} memory_region;

extern "C" void kmain(multiboot_info_t *p_boot_info) {
	clear_screen();
	kprint("We are in the kernel!\nWelcome to ZincOS!\n\n");

	memory_region *region=(memory_region *)p_boot_info->mmap_addr;

	for (int i=0; i<15; ++i) {

			//! sanity check; if type is > 4 mark it reserved
			if (region[i].type>4)
				region[i].type=1;

			//! if start address is 0, there is no more entries, break out
			if (i>0 && region[i].startLo==0)
				break;
			if (region[i].type==1){
				kprint("Available region! Start: ");
				char str[20];
				int_to_ascii(region[i].startLo,str);
				kprint(str);
				kprint(" Size: ");
				int_to_ascii(region[i].sizeLo,str);
				kprint(str);
				kprint("\n");
			}

			//! display entry
			//DebugPrintf ("region %i: start: 0x%x%x length (bytes): 0x%x%x type: %i (%s)\n", i,
				//region[i].startHi, region[i].startLo,
				//region[i].sizeHi,region[i].sizeLo,
				//region[i].type, strMemoryTypes[region[i].type-1]);

			//! if region is avilable memory, initialize the region for use
			//if (region[i].type==1)
				//pmmngr_init_region (region[i].startLo, region[i].sizeLo);
		}

	gdt_install();
  isr_irq_install();

	asm volatile("sti");
	//init_timer(50);
	init_keyboard();
}
