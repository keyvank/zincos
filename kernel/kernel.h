#pragma once

#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/memory.h"
#include "kernel/heap.h"
#include "kernel/process.h"
#include "kernel/terminal.h"
#include "kernel/vfs.h"
#include "libcpp/linked_list.h"

#define KERNEL_HEAP_SIZE_IN_PAGES (32 * _MB / 4096)

class kernel;

extern kernel *_kernel;

class _new_delete_initializer {
public:
  _new_delete_initializer(kernel * const p_kernel) {
    _kernel = p_kernel;
  }
};

class kernel {
  friend class process;
  friend class thread;
  friend class terminal;
  friend addr_t operator new(long unsigned int const p_size);
  friend addr_t operator new[](long unsigned int const p_size);
  friend void operator delete(addr_t const p_address);
  friend void operator delete[](addr_t const p_address);
  friend void debug(char_t const * const p_string);
  friend void debug(u32_t const p_integer);
private:
  memory m_memory;
  heap m_heap;
  _new_delete_initializer m_new_delete_initializer;
  vfs m_vfs;
  terminal m_terminal;
  terminal *m_active_terminal;

  page_directory_t *m_identity_page_directory;
  page_table_t *m_identity_page_tables;
  page_directory_t *m_user_page_directory;
  page_table_t *m_user_page_tables;

  size_t m_process_index;
  size_t m_thread_index;
  linked_list<process *> m_processes;
  linked_list<terminal *> m_terminals;

  bool m_userland;

  void create_process(addr_t const p_program);
  void task_switch();
  void terminate_process(size_t const p_index);

  void panic(char_t const * const p_message);
public:
  kernel(multiboot_info_t const &p_multiboot_info);
  kernel(kernel const &) = delete;
  kernel(kernel &&) = delete;
  kernel &operator=(kernel const &) = delete;
  kernel &operator=(kernel &&) = delete;

  void timer_handler(registers_t const p_registers);
  void keyboard_handler(registers_t const p_registers);
  void page_fault_handler(registers_t const p_registers);
  void sys(registers_t const);
  int sys_exit(u32_t const);
  int sys_write(char_t const * const);
  int sys_read(char_t * const, size_t const);
  int sys_alloc(size_t const, addr_t * const);
  int sys_thread(addr_t const);
};
