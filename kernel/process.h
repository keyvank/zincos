#pragma once

#include "cpu/types.h"
#include "kernel/paging.h"
#include "libcpp/array_list.h"
#include "kernel/terminal.h"
class kernel;

enum process_state_t { process_state_running, process_state_ready, process_state_blocked };
enum thread_state_t { thread_state_running, thread_state_ready, thread_state_blocked };

class process;
class thread {
  friend process;
  friend kernel;
private:
  process &m_parent;
  addr_t m_stack; // Virtual address of stack (Grows downwards)
  array_list<addr_t> m_used_blocks;
  cpu_state_t m_cpu_state;
  thread_state_t state;
  bool m_is_main;
public:
  thread(process &p_parent, addr_t const p_eip, bool const p_is_main);
  thread(thread const &) = default;
  thread(thread &&) = delete;
  thread &operator=(thread const &) = default;
  thread &operator=(thread &&) = delete;
  ~thread();
};

class process {
  friend thread;
  friend kernel;
  friend class terminal;
private:
  kernel &m_kernel;
  page_directory_t *m_page_directory;
  array_list<addr_t> m_used_blocks;
  array_list<addr_t> m_stack_blocks;
  u32_t m_heap_address;
  terminal *m_terminal;

  string m_input_buffer;
  char_t *m_sys_read_buffer;
  size_t m_sys_read_count;
  void flush_input_buffer();

  void create_thread(addr_t const p_eip, bool const p_is_main);

public:

  u32_t id;
  process_state_t state;
  array_list<thread *> threads;
  addr_t expand_heap(u32_t const p_pages);
  void terminate_thread(size_t const p_index);

  process(kernel &p_kernel, terminal *p_terminal, addr_t const p_program);
  process(process const &) = delete;
  process(process &&) = delete;
  process &operator=(process const &) = delete;
  process &operator=(process &&) = delete;
  ~process();
};
