#include "kernel/kernel.h"
#include "drivers/vga.h"
#include "kernel/util.h"
#include "cpu/isr.h"
#include "cpu/idt.h"
#include "cpu/gdt.h"
#include "cpu/timer.h"
#include "drivers/keyboard.h"
#include "kernel/multiboot.h"
#include "kernel/memory.h"
#include "cpu/asmutil.h"
#include "kernel/paging.h"
#include "cpu/ports.h"

void keyboard_callback(registers_t const p_registers) {
    UNUSED(p_registers);
    /* The PIC leaves us the scancode in port 0x60 */
    u8_t scancode = port_byte_in(0x60);
    char sc_ascii[16];
    int_to_ascii(scancode, sc_ascii);
    kprint("Keyboard scancode: ");
    kprint(sc_ascii);
    kprint(", ");
    print_letter(scancode);
    kprint("\n");
}

u32_t tick = 0;
void timer_callback(registers_t const p_registers) {
    UNUSED(p_registers);
    // Do nothing
}

void page_fault_callback(registers_t const p_registers) {
    UNUSED(p_registers);
    kprint("Page fault!\n");
    // Do nothing
}

kernel::kernel(multiboot_info_t const &p_multiboot_info) : m_memory(get_best_region(p_multiboot_info)), m_page_directory(NULL), m_page_tables(NULL) {
  clear_screen();
	kprint("We are in the kernel!\nWelcome to ZincOS!\n\n");

  if(this->m_memory.get_block_count() == 0) {
    kprint("Cannot initialize memory!");
    return;
  }

  kprint(this->m_memory.get_block_count());
  kprint(" memory blocks initialized!\n");

  this->m_page_directory = reinterpret_cast<page_directory_t *>(this->m_memory.allocate_blocks(1)); // Allocate a block for Page Directory
  this->m_page_tables = reinterpret_cast<page_table_t *>(this->m_memory.allocate_blocks(1024)); // Allocate 1024 blocks for Page Tables
  if(this->m_page_directory == NULL || this->m_page_tables == NULL) {
    kprint("Cannot allocate memory for paging!");
    return;
  }

  // Enable Paging by Identity Mapping whole 4GB of memory
  for(size_t i = 0; i < 1024; i++) {
    for(size_t j = 0; j < 1024; j++)
      this->m_page_tables[i].entries[j] = ((i * 1024 + j)  * 0x1000) | PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER;
    this->m_page_directory->entries[i] = reinterpret_cast<u32_t>(&(this->m_page_tables[i])) | PAGE_DIRECTORY_ENTRY_PRESENT | PAGE_DIRECTORY_ENTRY_WRITABLE | PAGE_DIRECTORY_ENTRY_USER;
  }
  load_page_directory(reinterpret_cast<u32_t *>(this->m_page_directory));
  enable_paging();
  kprint("Paging enabled!\n");

	gdt_install();
  isr_irq_install();
  init_keyboard(keyboard_callback);
  init_timer(25,timer_callback);
  init_paging(page_fault_callback);
	asm volatile("sti");

  tss_set_kernel_stack(get_esp());
  enter_usermode();
  kprint("Welcome to Usermode! Notice that all pages are Usermode pages!\n");
}
