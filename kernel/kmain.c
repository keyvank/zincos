#include "drivers/display/vga/vga_text.h"
#include "kernel/util.h"
#include "cpu/isr.h"
#include "cpu/idt.h"

void kmain() {
	kprint("We are in the kernel!\nWelcome to ZincOS!\n\n");
  isr_install();
  /* Test the interrupts */
  __asm__ __volatile__("int $2");
  __asm__ __volatile__("int $3");
}
