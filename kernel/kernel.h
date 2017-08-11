#pragma once

#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/memory.h"
#include "kernel/heap.h"
#include "kernel/process.h"
#include "libcpp/linked_list.h"

#define KERNEL_HEAP_SIZE_IN_PAGES (32 * _MB / 4096)

class kernel {
  friend class process;
  friend class thread;
  friend addr_t operator new(long unsigned int const p_size);
  friend void operator delete(addr_t const p_address);
  friend void scheduler(registers_t const p_registers);
private:
  memory m_memory;
  heap m_heap;
  page_directory_t *m_identity_page_directory;
  page_table_t *m_identity_page_tables;
  page_directory_t *m_user_page_directory;
  page_table_t *m_user_page_tables;

  size_t m_process_index;
  linked_list<process *> m_processes;

  void create_process();
  void terminate_process(size_t const p_index);

public:
  kernel(multiboot_info_t const &p_multiboot_info);
  kernel(kernel const &) = delete;
  kernel(kernel &&) = delete;
  kernel &operator=(kernel const &) = delete;
  kernel &operator=(kernel &&) = delete;
};
