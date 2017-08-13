#include "cpu/types.h"
#include "kernel/heap.h"
#include "drivers/vga.h"
#include "kernel/util.h"

#define HEAP_HEADER_MAGIC (0x1234abcd)
#define HEAP_FOOTER_MAGIC (0xabcd1234)

typedef struct {
  u32_t magic;
  bool used;
  u32_t size;
} heap_header_t;

typedef struct {
  u32_t magic;
  heap_header_t *header;
} heap_footer_t;

heap::heap(u8_t * const p_address, size_t const p_size) : m_address(p_address), m_size(p_size) {
  // Padding with zero
  memory_set(p_address, 0, sizeof(heap_footer_t));
  memory_set(p_address + p_size - sizeof(heap_header_t), 0, sizeof(heap_header_t));
  m_address += sizeof(heap_footer_t);
  m_size -= sizeof(heap_footer_t) + sizeof(heap_header_t);

  heap_header_t *first_header = reinterpret_cast<heap_header_t *>(this->m_address);
  heap_footer_t *first_footer = reinterpret_cast<heap_footer_t *>(this->m_address + this->m_size - sizeof(heap_footer_t));
  first_header->magic = HEAP_HEADER_MAGIC;
  first_header->used = false;
  first_header->size = this->m_size - sizeof(heap_header_t) - sizeof(heap_footer_t);
  first_footer->magic = HEAP_FOOTER_MAGIC;
  first_footer->header = first_header;
}

//void heap::all_blocks(){
//  heap_header_t *curr_header = reinterpret_cast<heap_header_t *>(this->m_address);
//  while(curr_header->magic == HEAP_HEADER_MAGIC){
//    kprint(curr_header->size);
//    kprint(" ");
//    curr_header = reinterpret_cast<heap_header_t *>(reinterpret_cast<u8_t *>(curr_header) + sizeof(heap_header_t) + curr_header->size + sizeof(heap_footer_t));
//  }
//  kprint("\n");
//}

addr_t heap::allocate(u32_t const p_count) {
  heap_header_t *curr_header = reinterpret_cast<heap_header_t *>(this->m_address);
  while(curr_header->magic == HEAP_HEADER_MAGIC && (curr_header->size < p_count || curr_header->used))
    curr_header = reinterpret_cast<heap_header_t *>(reinterpret_cast<u8_t *>(curr_header) + sizeof(heap_header_t) + curr_header->size + sizeof(heap_footer_t));
  if(curr_header->magic != HEAP_HEADER_MAGIC)
    return NULL;
  heap_footer_t *curr_footer = reinterpret_cast<heap_footer_t *>(reinterpret_cast<u8_t *>(curr_header) + sizeof(heap_header_t) + curr_header->size);
  if(curr_footer->magic != HEAP_FOOTER_MAGIC || curr_footer->header != curr_header)
    return NULL;
  if(curr_header->size > p_count + sizeof(heap_header_t) + sizeof(heap_footer_t)) {
    u32_t old_size = curr_header->size;
    heap_footer_t *new_footer = reinterpret_cast<heap_footer_t *>(reinterpret_cast<u8_t *>(curr_header) + sizeof(heap_header_t) + p_count);
    curr_header->size = p_count;
    curr_header->used = true;
    new_footer->magic = HEAP_FOOTER_MAGIC;
    new_footer->header = curr_header;
    heap_header_t *new_header = reinterpret_cast<heap_header_t *>(reinterpret_cast<u8_t *>(curr_header) + sizeof(heap_header_t) + p_count + sizeof(heap_footer_t));
    new_header->magic = HEAP_HEADER_MAGIC;
    new_header->used = false;
    new_header->size = old_size - p_count - sizeof(heap_header_t) - sizeof(heap_footer_t);
    curr_footer->header = new_header;
  }
  else
    curr_header->used = true;
  return (reinterpret_cast<u8_t *>(curr_header) + sizeof(heap_header_t));
}

void heap::free(addr_t const p_address) {
  heap_header_t *curr_header = reinterpret_cast<heap_header_t *>(reinterpret_cast<u8_t *>(p_address) - sizeof(heap_header_t));
  if(curr_header->magic != HEAP_HEADER_MAGIC)
    return;
  heap_header_t *left_most_header = curr_header;
  heap_header_t *right_most_header = curr_header;
  while(true) {
    heap_footer_t *left_footer = reinterpret_cast<heap_footer_t *>(reinterpret_cast<u8_t *>(left_most_header) - sizeof(heap_footer_t));
    if(left_footer->magic == HEAP_FOOTER_MAGIC && left_footer->header->magic == HEAP_HEADER_MAGIC && !left_footer->header->used)
      left_most_header = left_footer->header;
    else
      break;
  }
  while(true) {
    heap_header_t *right_header = reinterpret_cast<heap_header_t *>(reinterpret_cast<u8_t *>(right_most_header) + sizeof(heap_header_t) + right_most_header->size + sizeof(heap_footer_t));
    if(right_header->magic == HEAP_HEADER_MAGIC && !right_header->used)
      right_most_header = right_header;
    else
      break;
  }
  if(left_most_header == right_most_header)
    curr_header->used = false;
  else {
    heap_footer_t *right_most_footer = reinterpret_cast<heap_footer_t *>(reinterpret_cast<u8_t *>(right_most_header) + sizeof(heap_header_t) + right_most_header->size);
    left_most_header->used = false;
    left_most_header->size = reinterpret_cast<u8_t *>(right_most_footer) - reinterpret_cast<u8_t *>(left_most_header) - sizeof(heap_header_t);
    right_most_footer->header = left_most_header;
  }
}

size_t heap::get_size() {
  return this->m_size;
}
