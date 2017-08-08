#pragma once

#include "cpu/types.h"

class heap {
private:
  u8_t *m_address;
  size_t m_size;
public:
  heap(u8_t * const p_address, size_t const p_size);
  heap() = delete;
  heap(heap const &) = delete;
  heap(heap &&) = delete;
  heap &operator=(heap const &) = delete;
  heap &operator=(heap &&) = delete;

  addr_t allocate(u32_t const p_count);
  void free(addr_t const p_address);
  void all_blocks();
  size_t get_size();
};
