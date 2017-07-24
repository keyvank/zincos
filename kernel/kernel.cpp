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

memory_region get_available_region(multiboot_info_t const &p_multiboot_info){
  const size_t MAX_REGION_COUNT = 15;
  multiboot_memory_map_t *region=(multiboot_memory_map_t *)p_multiboot_info.mmap_addr;
  for(size_t i = 0; i < MAX_REGION_COUNT; i++) {
		if (region[i].type==1 && region[i].start_low > (64 * _KB)){
			memory_region reg{reinterpret_cast<addr_t>(region[i].start_low),region[i].size_low};
      return reg;
		}
	}
  return memory_region{0,0};
}

kernel::kernel(multiboot_info_t const &p_multiboot_info) : m_memory(get_available_region(p_multiboot_info)), m_page_directory(NULL), m_page_tables(NULL) {
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
    this->m_page_directory->entries[i] = reinterpret_cast<u32_t>(&(this->m_page_tables[i])) | 3;
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
