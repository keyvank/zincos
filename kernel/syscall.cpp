#include "kernel/syscall.h"
#include "drivers/vga.h"
#include "kernel/util.h"

extern "C" int syscall_0(const char *str){
  kprint(str);
  return 0;
}

static void *syscalls[1] =
{
   (void*)&syscall_0
};

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
