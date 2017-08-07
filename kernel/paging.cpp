#include "kernel/paging.h"
#include "kernel/util.h"
#include "cpu/isr.h"

void init_paging(isr_t const p_page_fault_handler) {
  register_interrupt_handler(14, p_page_fault_handler);
}

bool create_page_table(page_directory_t *p_directory, u32_t p_virtual, u32_t p_flags, memory &p_memory) {
  if (p_directory->entries[p_virtual >> 22] == 0) {
		addr_t block = p_memory.allocate_blocks(1);
		if (!block)
		  return false;
    memory_set(reinterpret_cast<u8_t*>(block), 0, 4096);
	  p_directory->entries[p_virtual >> 22] = ((page_table_entry_t) block) | p_flags;
		map_physical(p_directory, (u32_t)block, (u32_t)block, p_flags, p_memory);
  }
	return true;
}

void map_physical(page_directory_t *p_directory, u32_t p_virtual, u32_t p_physical, u32_t p_flags, memory &p_memory) {
  if (p_directory->entries[p_virtual >> 22] == 0)
    create_page_table(p_directory, p_virtual, p_flags, p_memory);
  ((u32_t*) (p_directory->entries[p_virtual >> 22] & ~0xfff))[p_virtual << 10 >> 10 >> 12] = p_physical | p_flags;
}

void* get_physical_address(page_directory_t const *p_directory, u32_t p_virtual) {
  if (p_directory->entries[p_virtual >> 22] == 0)
    return 0;
  return reinterpret_cast<addr_t>(((u32_t*) (p_directory->entries[p_virtual >> 22] & ~0xfff))[p_virtual << 10 >> 10 >> 12]);
}

page_directory_t *create_address_space(memory &p_memory) {
  page_directory_t *dir = 0;
  dir = reinterpret_cast<page_directory_t *>(p_memory.allocate_blocks(1));
  if (!dir)
    return 0;
  memory_set(reinterpret_cast<u8_t *>(dir), 0, sizeof (page_directory_t));
  return dir;
}
