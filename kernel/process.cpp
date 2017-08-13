#include "kernel/kernel.h"
#include "kernel/process.h"
#include "kernel/util.h"
#include "drivers/disk.h"
#include "drivers/vga.h"

#define APPS_START_SECTOR (116)
#define USER_ENTRY_ADDRESS (8 * _MB)
#define USER_STACK_ADDRESS (1 * _GB)
#define USER_STACK_SIZE_IN_PAGES (2)
#define USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES (2)

process::process(kernel &p_kernel, terminal *p_terminal) : m_kernel(p_kernel), m_terminal(p_terminal) {
  this->threads = new array_list<thread>(this->m_kernel.m_heap);
  this->m_used_blocks = new array_list<addr_t>(this->m_kernel.m_heap);
  this->id = 1;
  this->m_page_directory = reinterpret_cast<page_directory_t *>(this->m_kernel.m_memory.allocate_blocks(1));
  memory_copy(reinterpret_cast<u8_t *>(this->m_kernel.m_user_page_directory),reinterpret_cast<u8_t *>(this->m_page_directory),sizeof(page_directory_t));
  this->state = process_state_t::process_state_running;

  thread main_thread;
  main_thread.m_used_blocks = new array_list<addr_t>(this->m_kernel.m_heap);
  main_thread.m_parent = this;
  main_thread.state = thread_state_t::thread_state_running;

  addr_t stack = this->m_kernel.m_memory.allocate_blocks(USER_STACK_SIZE_IN_PAGES);
  main_thread.m_stack = (addr_t)USER_STACK_ADDRESS;
  for(size_t i = 0; i < USER_STACK_SIZE_IN_PAGES; i++) {
    // map_physical returns the corresponding block address if a new page table should be created!
    this->m_used_blocks->add(reinterpret_cast<addr_t>((reinterpret_cast<u8_t *>(stack) + 4096 * i)));
    addr_t block = map_physical(this->m_page_directory, this->m_kernel.m_user_page_directory, reinterpret_cast<u32_t>((u8_t*)main_thread.m_stack - (i + 1) * 4096), reinterpret_cast<u32_t>(stack) + 4096 * i, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel.m_memory);
    if(block) this->m_used_blocks->add(block);
  }

  addr_t image = this->m_kernel.m_memory.allocate_blocks(USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES);
  read_sectors(APPS_START_SECTOR, 1, (u8_t*)image);
  for(size_t i = 0; i < USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES; i++) {
    addr_t block = map_physical(this->m_page_directory, this->m_kernel.m_user_page_directory, USER_ENTRY_ADDRESS + 4096 * i, reinterpret_cast<u32_t>(image) + 4096 * i, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel.m_memory);
    this->m_used_blocks->add(reinterpret_cast<addr_t>((reinterpret_cast<u8_t *>(image) + 4096 * i)));
    if(block) this->m_used_blocks->add(block);
  }

  memory_set(reinterpret_cast<u8_t *>(&main_thread.m_cpu_state), 0, sizeof (cpu_state_t));
  main_thread.m_cpu_state.esp = reinterpret_cast<u32_t>(main_thread.m_stack);
  main_thread.m_cpu_state.ebp = main_thread.m_cpu_state.esp;
  main_thread.m_cpu_state.eip = USER_ENTRY_ADDRESS;
  main_thread.m_cpu_state.flags = 0x200; // Interrupt flag set
  this->threads->add(main_thread);
}

process::~process() {
  this->m_kernel.m_memory.free_block(this->m_page_directory);
  for(size_t i = 0; i < this->m_used_blocks->get_size(); i++)
    this->m_kernel.m_memory.free_block((*this->m_used_blocks)[i]);
  delete this->m_used_blocks;
  delete this->threads;
}

thread::~thread() {
  for(size_t j = 0; j < this->m_used_blocks->get_size(); j++)
    this->m_parent->m_kernel.m_memory.free_block((*this->m_used_blocks)[j]);
  delete this->m_used_blocks;
}
