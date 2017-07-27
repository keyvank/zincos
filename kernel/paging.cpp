#include "kernel/paging.h"
#include "cpu/isr.h"

void init_paging(isr_t const p_page_fault_handler) {
  register_interrupt_handler(14, p_page_fault_handler);
}
