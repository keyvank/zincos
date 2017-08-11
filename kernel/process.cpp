#include "kernel/kernel.h"
#include "kernel/process.h"
#include "kernel/util.h"
#include "drivers/disk.h"

#define APPS_START_SECTOR (116)
#define USER_ENTRY_ADDRESS (8 * _MB)
#define USER_STACK_ADDRESS (1 * _GB)
#define USER_STACK_SIZE_IN_PAGES (2)
#define USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES (2)

process::process(kernel * const p_kernel) : m_kernel(p_kernel) {
  this->threads = new array_list<thread>(this->m_kernel->m_heap);
  this->used_blocks = new array_list<addr_t>(this->m_kernel->m_heap);
  this->id = 1;
  this->page_directory = reinterpret_cast<page_directory_t *>(this->m_kernel->m_memory.allocate_blocks(1));
  memory_copy(reinterpret_cast<u8_t *>(this->m_kernel->m_user_page_directory),reinterpret_cast<u8_t *>(this->page_directory),sizeof(page_directory_t));
  this->priority = 1;
  this->state = PROCESS_STATE_RUNNING;

  thread main_thread;
  main_thread.used_blocks = new array_list<addr_t>(this->m_kernel->m_heap);
  main_thread.parent = this;
  main_thread.priority = 1;
  main_thread.state = PROCESS_STATE_RUNNING;

  addr_t stack = this->m_kernel->m_memory.allocate_blocks(USER_STACK_SIZE_IN_PAGES);
  main_thread.stack = (addr_t)USER_STACK_ADDRESS;
  for(size_t i = 0; i < USER_STACK_SIZE_IN_PAGES; i++) {
    // map_physical returns the corresponding block address if a new page table should be created!
    this->used_blocks->add(reinterpret_cast<addr_t>((reinterpret_cast<u8_t *>(stack) + 4096 * i)));
    addr_t block = map_physical(this->page_directory, this->m_kernel->m_user_page_directory, reinterpret_cast<u32_t>((u8_t*)main_thread.stack - (i + 1) * 4096), reinterpret_cast<u32_t>(stack) + 4096 * i, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel->m_memory);
    if(block) this->used_blocks->add(block);
  }

  addr_t image = this->m_kernel->m_memory.allocate_blocks(USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES);
  read_sectors(APPS_START_SECTOR, 2, (u8_t*)image);
  for(size_t i = 0; i < USER_EXECUTABLE_IMAGE_SIZE_IN_PAGES; i++) {
    addr_t block = map_physical(this->page_directory, this->m_kernel->m_user_page_directory, USER_ENTRY_ADDRESS + 4096 * i, reinterpret_cast<u32_t>(image) + 4096 * i, PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_kernel->m_memory);
    this->used_blocks->add(reinterpret_cast<addr_t>((reinterpret_cast<u8_t *>(image) + 4096 * i)));
    if(block) this->used_blocks->add(block);
  }

  memory_set(reinterpret_cast<u8_t *>(&main_thread.frame), 0, sizeof (cpu_state_t));
  main_thread.frame.esp = reinterpret_cast<u32_t>(main_thread.stack);
  main_thread.frame.ebp = main_thread.frame.esp;
  main_thread.frame.eip = USER_ENTRY_ADDRESS;
  main_thread.frame.flags = 0x200; // Interrupt flag set
  this->threads->add(main_thread);
}

process::~process() {
  this->m_kernel->m_memory.free_block(this->page_directory);
  for(size_t i = 0; i < this->used_blocks->get_size(); i++)
    this->m_kernel->m_memory.free_block((*this->used_blocks)[i]);
  delete this->used_blocks;
  delete this->threads;
}

thread::~thread() {
  for(size_t j = 0; j < this->used_blocks->get_size(); j++)
    this->parent->m_kernel->m_memory.free_block((*this->used_blocks)[j]);
  delete this->used_blocks;
}
