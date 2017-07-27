#include "kernel/kernel.h"
#include "drivers/vga.h"
#include "kernel/util.h"
#include "cpu/isr.h"
#include "cpu/idt.h"
#include "cpu/gdt.h"
#include "drivers/keyboard.h"
#include "kernel/multiboot.h"
#include "kernel/memory.h"
#include "cpu/asmutil.h"
#include "kernel/paging.h"

kernel::kernel(multiboot_info_t const &p_multiboot_info) : m_memory(get_best_region(p_multiboot_info)), m_page_directory(NULL), m_page_tables(NULL) {
  clear_screen();
	kprint("We are in the kernel!\nWelcome to ZincOS!\n\n");

  if(this->m_memory.get_block_count() == 0) {
    kprint("Cannot initialize memory!");
    return;
  }

  kprint(this->m_memory.get_block_count());
  kprint(" memory blocks initialized!\n");

  this->m_page_directory = reinterpret_cast<page_directory_t *>(this->m_memory.force_allocate_block(1)); // Allocate a block for Page Directory
  this->m_page_tables = reinterpret_cast<page_table_t *>(this->m_memory.force_allocate_blocks(2,1024)); // Allocate 1024 blocks for Page Tables
  if(this->m_page_directory == NULL || this->m_page_tables == NULL) {
    kprint("Cannot allocate memory for paging!");
    return;
  }

  // Enable Paging by Identity Mapping whole 4GB of memory
  for(size_t i = 0; i < 1024; i++) {
    for(size_t j = 0; j < 1024; j++)
      this->m_page_tables[i].entries[j] = ((i * 1024 + j)  * 0x1000) | PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE;
    this->m_page_directory->entries[i] = reinterpret_cast<u32_t>(&(this->m_page_tables[i])) | PAGE_DIRECTORY_ENTRY_PRESENT | PAGE_DIRECTORY_ENTRY_WRITABLE;
  }
  load_page_directory(reinterpret_cast<u32_t *>(this->m_page_directory));
  enable_paging();
  kprint("Paging enabled!\n");

	gdt_install();
  isr_irq_install();
	asm volatile("sti");
	init_keyboard();

	//init_timer(50);
}
