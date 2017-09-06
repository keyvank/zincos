#include "kernel/kernel.h"
#include "kernel/process.h"
#include "kernel/util.h"
#include "drivers/disk.h"
#include "drivers/vga.h"

#define APPS_START_SECTOR (116)
#define USER_ENTRY_ADDRESS (8 * _MB)
#define USER_STACK_ADDRESS (1 * _GB)
#define USER_HEAP_ADDRESS (512 * _MB)
#define USER_STACK_SIZE_IN_PAGES (2)
#define USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES (8)

void idle() { while(true); }

process::process(kernel &p_kernel, terminal *p_terminal, addr_t const p_program) :
    m_kernel(p_kernel),
    m_used_blocks(),
    m_stack_blocks(),
    m_heap_address(USER_HEAP_ADDRESS),
    m_terminal(p_terminal),
    m_input_buffer(),
    m_sys_read_buffer(nullptr),
    m_sys_read_count(0) {
  this->id = 1;
  this->m_page_directory = reinterpret_cast<page_directory_t *>(this->m_kernel.m_memory.allocate_blocks(1));
  memory_copy(reinterpret_cast<u8_t *>(this->m_kernel.m_user_page_directory),reinterpret_cast<u8_t *>(this->m_page_directory),sizeof(page_directory_t));
  this->state = process_state_t::process_state_running;

  addr_t image = this->m_kernel.m_memory.allocate_blocks(USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES);
  if(p_program)
    read_sectors(APPS_START_SECTOR + reinterpret_cast<u32_t>(p_program) - 1, 8 * USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES, reinterpret_cast<u8_t *>(image));
  else
    memory_copy(reinterpret_cast<u8_t *>(idle), reinterpret_cast<u8_t *>(image), 4096 * USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES);
  for(size_t i = 0; i < USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES; i++) {
    addr_t block = map_physical(this->m_page_directory, this->m_kernel.m_user_page_directory, USER_ENTRY_ADDRESS + 4096 * i, reinterpret_cast<u32_t>(image) + 4096 * i, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel.m_memory);
    this->m_used_blocks.add(reinterpret_cast<addr_t>((reinterpret_cast<u8_t *>(image) + 4096 * i)));
    if(block) this->m_used_blocks.add(block);
  }

  this->create_thread(reinterpret_cast<addr_t>(USER_ENTRY_ADDRESS), true); // Adding the main thread
}

process::~process() {
  this->m_kernel.m_memory.free_block(this->m_page_directory);
  for(size_t i = 0; i < this->m_used_blocks.get_size(); i++)
    this->m_kernel.m_memory.free_block(this->m_used_blocks[i]);
}

thread::thread(process &p_parent, addr_t const p_eip, bool const p_is_main) :
    m_parent(p_parent),
    m_used_blocks(),
    m_is_main(p_is_main) {
  this->state = thread_state_t::thread_state_running;

  addr_t stack = this->m_parent.m_kernel.m_memory.allocate_blocks(USER_STACK_SIZE_IN_PAGES);
  u32_t vstack = USER_STACK_ADDRESS;
  while(this->m_parent.m_stack_blocks.contains(reinterpret_cast<addr_t>(vstack))) vstack -= 4096 * USER_STACK_SIZE_IN_PAGES;
  this->m_stack = reinterpret_cast<addr_t>(vstack);
  this->m_parent.m_stack_blocks.add(this->m_stack);

  for(size_t i = 0; i < USER_STACK_SIZE_IN_PAGES; i++) {
    // map_physical returns the corresponding block address if a new page table should be created!
    this->m_used_blocks.add(reinterpret_cast<addr_t>((reinterpret_cast<u8_t *>(stack) + 4096 * i)));
    addr_t block = map_physical(this->m_parent.m_page_directory, this->m_parent.m_kernel.m_user_page_directory, reinterpret_cast<u32_t>((u8_t*)this->m_stack - (i + 1) * 4096), reinterpret_cast<u32_t>(stack) + 4096 * i, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_parent.m_kernel.m_memory);
    if(block) this->m_used_blocks.add(block);
  }

  memory_set(reinterpret_cast<u8_t *>(&this->m_cpu_state), 0, sizeof (cpu_state_t));
  this->m_cpu_state.esp = reinterpret_cast<u32_t>(this->m_stack);
  this->m_cpu_state.ebp = this->m_cpu_state.esp;
  this->m_cpu_state.eip = reinterpret_cast<u32_t>(p_eip);
  this->m_cpu_state.flags = 0x200; // Interrupt flag set
}

thread::~thread() {
  this->m_parent.m_stack_blocks.remove(this->m_parent.m_stack_blocks.find(this->m_stack));
  for(size_t j = 0; j < this->m_used_blocks.get_size(); j++)
    this->m_parent.m_kernel.m_memory.free_block(this->m_used_blocks[j]);
}

void process::flush_input_buffer() {
  while(this->m_input_buffer.get_length() > 0 && this->m_sys_read_count > 0) {
    *this->m_sys_read_buffer = this->m_input_buffer.peek_char();
    this->m_sys_read_buffer++;
    this->m_sys_read_count--;
  }
  if(this->m_sys_read_count == 0)
    this->state = process_state_t::process_state_running;
}

addr_t process::expand_heap(u32_t const p_pages) {
  addr_t ret = reinterpret_cast<addr_t>(this->m_heap_address);
  for(size_t i = 0; i < p_pages; i++) {
    addr_t new_block = this->m_kernel.m_memory.allocate_blocks(1);
    this->m_used_blocks.add(new_block);
    page_table_t *tbl = map_physical(this->m_page_directory, this->m_kernel.m_user_page_directory, this->m_heap_address, reinterpret_cast<u32_t>(new_block),PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel.m_memory);
    if(tbl) this->m_used_blocks.add(reinterpret_cast<addr_t>(tbl));
    this->m_heap_address += 4096;
  }
  return ret;
}

void process::create_thread(addr_t const p_eip, bool const p_is_main) {
  thread *new_thread = new thread(*this, p_eip, p_is_main);
  this->threads.add(new_thread);
}

void process::terminate_thread(size_t const p_index) {
  delete this->threads[p_index];
  this->threads.remove(p_index);
}
