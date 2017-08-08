#pragma once

#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/memory.h"
#include "kernel/process.h"

class kernel {
private:
  memory m_memory;
  page_directory_t *m_identity_page_directory;
  page_table_t *m_identity_page_tables;
  page_directory_t *m_user_page_directory;
  page_table_t *m_user_page_tables;

  volatile process_t *m_current_process;
  volatile process_t *m_process_queue;
  page_directory_t *m_current_directory;

  void create_process();
  void terminate_process();

public:
  kernel(multiboot_info_t const &p_multiboot_info);
  kernel(kernel const &) = delete;
  kernel(kernel &&) = delete;
  kernel &operator=(kernel const &) = delete;
  kernel &operator=(kernel &&) = delete;
};
