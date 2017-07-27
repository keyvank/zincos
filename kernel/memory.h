#pragma once

#include "kernel/multiboot.h"
#include <libcpp/list.h>

#define NULL (0)

struct memory_region {
  addr_t base;
  size_t size;
};

class memory {
private:
  u8_t *m_bitmap_block_addr;
  size_t m_bitmap_block_count;
  u8_t *m_data_block_addr;
  size_t m_data_block_count;
public:
  memory(memory_region const &);
  memory() = delete;
  memory(memory const &) = delete;
  memory(memory &&) = delete;
  memory &operator=(memory const &) = delete;
  memory &operator=(memory &&) = delete;

  size_t get_block_count() const;

  inline void mark_block_free(u32_t const p_block);
  inline void mark_block_used(u32_t const p_block);
  inline bool is_block_used(u32_t const p_block) const;

  addr_t allocate_block();
  addr_t force_allocate_block(u32_t const p_block);
  addr_t force_allocate_blocks(u32_t const p_block, u32_t const p_count);
  void free_block(addr_t const p_address);
};

inline void memory::mark_block_used(u32_t const p_block) {
  reinterpret_cast<u32_t *>(this->m_data_block_addr)[p_block / 32] |= (1 << (p_block % 32));
}
inline void memory::mark_block_free(u32_t const p_block) {
  reinterpret_cast<u32_t *>(this->m_data_block_addr)[p_block / 32] &= ~(1 << (p_block % 32));
}
inline bool memory::is_block_used(u32_t const p_block) const {
 return reinterpret_cast<u32_t *>(this->m_data_block_addr)[p_block / 32] &  (1 << (p_block % 32));
}

memory_region get_best_region(multiboot_info_t const &p_multiboot_info);
