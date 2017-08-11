#pragma once

#include "cpu/types.h"
#include "kernel/paging.h"
#include "libcpp/array_list.h"
class kernel;

#define PROCESS_STATE_RUNNING (0)
#define PROCESS_STATE_READY (1)
#define PROCESS_STATE_BLOCKED (2)

class process;
class thread {
  friend process;
private:
public:
  process *parent;
  addr_t stack; // Virtual address of stack (Grows downwards)
  u8_t priority;
  int state;
  cpu_state_t frame;
  array_list<addr_t> *used_blocks;
  ~thread();
};

class process {
  friend thread;
private:
  kernel *m_kernel;
public:
  u32_t id;
  u8_t priority;
  page_directory_t *page_directory;
  u8_t state;
  array_list<addr_t> *used_blocks;
  array_list<thread> *threads;
  process(kernel * const p_kernel);
  ~process();
};
