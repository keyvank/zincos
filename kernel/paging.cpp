#include "kernel/paging.h"
#include "kernel/util.h"
#include "cpu/isr.h"

void init_paging(isr_t const p_page_fault_handler) {
  register_interrupt_handler(14, p_page_fault_handler);
}

page_table_t *get_table(page_directory_t *p_directory, u32_t p_frame) {
  return ((page_table_t*) (p_directory->entries[p_frame] & ~0xfff));
}

void clone_page_table(page_table_t *p_source, page_table_t *p_destination) {
  memory_copy(reinterpret_cast<u8_t *>(p_source),reinterpret_cast<u8_t *>(p_destination),sizeof(page_table_t));
}

// If a Page Table needs to be created, returns it.
page_table_t *map_physical(page_directory_t *p_directory, page_directory_t *p_template, u32_t p_virtual, u32_t p_physical, u32_t p_flags, memory &p_memory) {
  u32_t directory_frame = p_virtual >> 22;
  u32_t table_frame = p_virtual << 10 >> 10 >> 12;
  page_table_t *clone = NULL;
  if (p_directory->entries[directory_frame] == p_template->entries[directory_frame]) {
    clone = reinterpret_cast<page_table_t *>(p_memory.allocate_blocks(1));
    clone_page_table(get_table(p_template, directory_frame), clone);
    p_directory->entries[directory_frame] = reinterpret_cast<u32_t>(clone) | p_flags;
  } else
    p_directory->entries[directory_frame] |= p_flags;
  get_table(p_directory, directory_frame)->entries[table_frame] = p_physical | p_flags;
  return clone;
}

u32_t get_physical(page_directory_t *p_directory, u32_t p_virtual) {
  u32_t directory_frame = p_virtual >> 22;
  u32_t table_frame = p_virtual << 10 >> 10 >> 12;
  return ((get_table(p_directory, directory_frame)->entries[table_frame]) >> 12 << 12) + (p_virtual << 20 >> 20);
}
