#include "drivers/display/vga/vga_text.h"

void kmain() {
	clear_screen();
	kprint("We are in the kernel now!\nWelcome to ZincOS!\n > ");
}
