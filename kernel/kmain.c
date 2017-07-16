#include "drivers/display/vga/vga_text.h"
#include "kernel/util.h"
#include "cpu/isr.h"
#include "cpu/idt.h"

void kmain() {
	clear_screen();
	kprint("We are in the kernel!\nWelcome to ZincOS!\n\n");
  isr_irq_install();

	asm volatile("sti");
	init_timer(50);
	/* Comment out the timer IRQ handler to read
	* the keyboard IRQs easier */
	init_keyboard();
  /* Test the interrupts */
  __asm__ __volatile__("int $2");
  __asm__ __volatile__("int $3");
}
