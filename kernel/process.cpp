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

process::process(kernel &p_kernel, terminal *p_terminal, addr_t const p_program) : m_kernel(p_kernel), m_heap_address(USER_HEAP_ADDRESS), m_terminal(p_terminal), m_input_buffer(new string()), m_sys_read_buffer(nullptr), m_sys_read_count(0) {
  this->threads = new array_list<thread *>();
  this->m_used_blocks = new array_list<addr_t>();
  this->id = 1;
  this->m_page_directory = reinterpret_cast<page_directory_t *>(this->m_kernel.m_memory.allocate_blocks(1));
  memory_copy(reinterpret_cast<u8_t *>(this->m_kernel.m_user_page_directory),reinterpret_cast<u8_t *>(this->m_page_directory),sizeof(page_directory_t));
  this->state = process_state_t::process_state_running;

  thread *main_thread = new thread(*this);
  main_thread->state = thread_state_t::thread_state_running;

  addr_t stack = this->m_kernel.m_memory.allocate_blocks(USER_STACK_SIZE_IN_PAGES);
  main_thread->m_stack = (addr_t)USER_STACK_ADDRESS;
  for(size_t i = 0; i < USER_STACK_SIZE_IN_PAGES; i++) {
    // map_physical returns the corresponding block address if a new page table should be created!
    this->m_used_blocks->add(reinterpret_cast<addr_t>((reinterpret_cast<u8_t *>(stack) + 4096 * i)));
    addr_t block = map_physical(this->m_page_directory, this->m_kernel.m_user_page_directory, reinterpret_cast<u32_t>((u8_t*)main_thread->m_stack - (i + 1) * 4096), reinterpret_cast<u32_t>(stack) + 4096 * i, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel.m_memory);
    if(block) this->m_used_blocks->add(block);
  }

  addr_t image = this->m_kernel.m_memory.allocate_blocks(USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES);
  if(p_program)
    read_sectors(APPS_START_SECTOR + reinterpret_cast<u32_t>(p_program) - 1, 8 * USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES, reinterpret_cast<u8_t *>(image));
  else
    memory_copy(reinterpret_cast<u8_t *>(idle), reinterpret_cast<u8_t *>(image), 4096 * USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES);
  for(size_t i = 0; i < USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES; i++) {
    addr_t block = map_physical(this->m_page_directory, this->m_kernel.m_user_page_directory, USER_ENTRY_ADDRESS + 4096 * i, reinterpret_cast<u32_t>(image) + 4096 * i, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel.m_memory);
    this->m_used_blocks->add(reinterpret_cast<addr_t>((reinterpret_cast<u8_t *>(image) + 4096 * i)));
    if(block) this->m_used_blocks->add(block);
  }

  memory_set(reinterpret_cast<u8_t *>(&main_thread->m_cpu_state), 0, sizeof (cpu_state_t));
  main_thread->m_cpu_state.esp = reinterpret_cast<u32_t>(main_thread->m_stack);
  main_thread->m_cpu_state.ebp = main_thread->m_cpu_state.esp;
  main_thread->m_cpu_state.eip = USER_ENTRY_ADDRESS;
  main_thread->m_cpu_state.flags = 0x200; // Interrupt flag set
  this->threads->add(main_thread);
}

process::~process() {
  this->m_kernel.m_memory.free_block(this->m_page_directory);
  for(size_t i = 0; i < this->m_used_blocks->get_size(); i++)
    this->m_kernel.m_memory.free_block((*this->m_used_blocks)[i]);
  delete this->m_used_blocks;
  delete this->threads;
  delete this->m_input_buffer;
}

thread::thread(process &p_parent) :
    m_parent(p_parent),
    m_used_blocks(new array_list<addr_t>()) {

}

thread::~thread() {
  for(size_t j = 0; j < this->m_used_blocks->get_size(); j++)
    this->m_parent.m_kernel.m_memory.free_block((*this->m_used_blocks)[j]);
  delete this->m_used_blocks;
}

void process::flush_input_buffer() {
  while(this->m_input_buffer->get_length() > 0 && this->m_sys_read_count > 0) {
    *this->m_sys_read_buffer = this->m_input_buffer->peek_char();
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
    this->m_used_blocks->add(new_block);
    page_table_t *tbl = map_physical(this->m_page_directory, this->m_kernel.m_user_page_directory, this->m_heap_address, reinterpret_cast<u32_t>(new_block),PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel.m_memory);
    if(tbl) this->m_used_blocks->add(reinterpret_cast<addr_t>(tbl));
    this->m_heap_address += 4096;
  }
  return ret;
}
