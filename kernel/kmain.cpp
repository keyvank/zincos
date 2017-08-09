#include "kernel/kernel.h"
#include "kernel/multiboot.h"

extern "C" void kmain(multiboot_info_t const *p_boot_info) {
	kernel kern(*p_boot_info);
	while(true);
}
