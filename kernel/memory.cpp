#include "kernel/memory.h"
#include "kernel/util.h"

#define BLOCK_SIZE (4096)
#define BITS_PER_BYTE (8)

memory::memory(memory_region const &p_region) {
  u32_t first = align_right(reinterpret_cast<u32_t>(p_region.base), BLOCK_SIZE);
  u32_t last = align_left(reinterpret_cast<u32_t>(p_region.base) + p_region.size, BLOCK_SIZE);
  u32_t size = last - first;
  u32_t block_count = size / BLOCK_SIZE; /* Size is divisible by BLOCK_SIZE */
  u32_t blocks_needed = div_ceil(div_ceil(block_count, BITS_PER_BYTE), BLOCK_SIZE);
  this->m_bitmap_block_addr = reinterpret_cast<u8_t *>(first);
  this->m_bitmap_block_count = blocks_needed;
  this->m_data_block_addr = reinterpret_cast<u8_t *>(first + blocks_needed * BLOCK_SIZE);
  this->m_data_block_count = block_count - blocks_needed;
  memory_set(this->m_bitmap_block_addr, 0, blocks_needed * BLOCK_SIZE);
}

size_t memory::get_block_count() const {
  return this->m_data_block_count;
}

u32_t memory::first_free() const {
  return -1;
}
