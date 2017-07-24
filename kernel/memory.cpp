#include "kernel/memory.h"
#include "kernel/util.h"
#include "kernel/memory.h"

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
  this->mark_block_used(0); // There is no block 0! So that even if memory starts from 0x0, NULL has a meaning!
}

size_t memory::get_block_count() const {
  return this->m_data_block_count;
}

// Returns NULL if there is no free block available
addr_t memory::allocate_block() {
  for(size_t i = 0; i < this->m_data_block_count; i++) {
    if(!this->is_block_used(i)) {
      this->mark_block_used(i);
      return this->m_data_block_addr + i * BLOCK_SIZE;
    }
  }
  return NULL;
}

// It doesn't free the block when the address is invalid. Fails silently.
void memory::free_block(addr_t const p_address) {
  u32_t offset = reinterpret_cast<u32_t>(reinterpret_cast<u8_t *>(p_address) - reinterpret_cast<u32_t>(this->m_data_block_addr));
  if(offset % BLOCK_SIZE == 0) {
    u32_t block = offset / BLOCK_SIZE;
    if(block < this->m_data_block_count)
      this->mark_block_free(block);
  }
}

addr_t memory::force_allocate_block(u32_t const p_block) {
  if(p_block < this->m_data_block_count) {
    this->mark_block_used(p_block);
    return this->m_data_block_addr + p_block * BLOCK_SIZE;
  }
  else
    return NULL;
}

addr_t memory::force_allocate_blocks(u32_t const p_block, u32_t const p_count) {
  u32_t end_block = p_block + p_count;
  if(p_block < this->m_data_block_count && end_block <= this->m_data_block_count) {
    for(u32_t i = p_block; i < end_block; i++)
      this->mark_block_used(i);
    return this->m_data_block_addr + p_block * BLOCK_SIZE;
  }
  else
    return NULL;
}
