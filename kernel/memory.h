#pragma once

#include <libcpp/list.h>

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
  u32_t first_free() const;

  inline void mark_free(u32_t const p_bit);
  inline void mark_used(u32_t const p_bit);
  inline bool is_used(u32_t const p_bit) const;
};

inline void memory::mark_used(u32_t const p_bit) {
  reinterpret_cast<u32_t *>(this->m_data_block_addr)[p_bit / 32] |= (1 << (p_bit % 32));
}
inline void memory::mark_free(u32_t const p_bit) {
  reinterpret_cast<u32_t *>(this->m_data_block_addr)[p_bit / 32] &= ~(1 << (p_bit % 32));
}
inline bool memory::is_used(u32_t const p_bit) const {
 return reinterpret_cast<u32_t *>(this->m_data_block_addr)[p_bit / 32] &  (1 << (p_bit % 32));
}
