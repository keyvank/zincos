#pragma once

#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/memory.h"

class kernel {
private:
  memory m_memory;
  page_directory_t *m_page_directory;
  page_table_t *m_page_tables;


public:
  kernel(multiboot_info_t const &p_multiboot_info);
  kernel(kernel const &) = delete;
  kernel(kernel &&) = delete;
  kernel &operator=(kernel const &) = delete;
  kernel &operator=(kernel &&) = delete;
};
