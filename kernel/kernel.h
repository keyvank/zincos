#pragma once

#include "kernel/multiboot.h"

class kernel {
private:
public:
  kernel(multiboot_info_t const &p_multiboot_info);
  kernel(kernel const &) = delete;
  kernel(kernel &&) = delete;
  kernel &operator=(kernel const &) = delete;
  kernel &operator=(kernel &&) = delete;
};
