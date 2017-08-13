#include "kernel/syscall.h"
#include "drivers/vga.h"
#include "kernel/util.h"

#define NUM_SYSCALLS (256)

static void *syscalls[NUM_SYSCALLS];

void set_syscall(size_t const p_index, void *p_function) {
  syscalls[p_index] = p_function;
}

void syscall_handler(registers_t p_registers) {
  UNUSED(p_registers);
  UNUSED(syscalls);

  void *location = syscalls[p_registers.eax];

  int ret;
  asm volatile (" \
    push %1; \
    push %2; \
    push %3; \
    push %4; \
    push %5; \
    call *%6; \
    pop %%ebx; \
    pop %%ebx; \
    pop %%ebx; \
    pop %%ebx; \
    pop %%ebx; \
  " : "=a" (ret) : "r" (p_registers.edi), "r" (p_registers.esi), "r" (p_registers.edx), "r" (p_registers.ecx), "r" (p_registers.ebx), "r" (location));
  p_registers.eax = ret;
}


void init_syscalls() {
  register_interrupt_handler(123, syscall_handler);
}
