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

// Returns NULL if there is no free block available
addr_t memory::allocate_blocks(u32_t const p_count) {
  u32_t curr_count = 0;
  for(size_t i = 0; i < this->m_data_block_count; i++) {
    if(!this->is_block_used(i))
      curr_count++;
    else
      curr_count = 0;
    if(curr_count == p_count) {
      size_t first = i + 1 - p_count;
      for(size_t j = 0; j < p_count; j++)
        this->mark_block_used(first + j);
      return this->m_data_block_addr + first * BLOCK_SIZE;
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

memory_region get_best_region(multiboot_info_t const &p_multiboot_info) {
  const size_t MAX_REGION_COUNT = 15;
  multiboot_memory_map_t *region=(multiboot_memory_map_t *)p_multiboot_info.mmap_addr;
  s32_t best_region_index = -1;
  for(size_t i = 0; i < MAX_REGION_COUNT; i++) {
    if (i > 0 && region[i].start_low == 0)
				break;
		if (region[i].type==1 && region[i].start_low > (64 * _KB)){
      if(best_region_index == -1 || region[i].size_low > region[best_region_index].size_low)
        best_region_index = i;
		}
	}
  if(best_region_index != -1) {
    memory_region reg{reinterpret_cast<addr_t>(region[best_region_index].start_low),region[best_region_index].size_low};
    return reg;
  }
  else
    return memory_region{0,0};
}
