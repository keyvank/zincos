#include "api.h"

void exit(unsigned int const p_exit_code) {
  int syscall_result;
  asm volatile("int $123" : "=a" (syscall_result) : "0" (0), "b" (p_exit_code));
}

void print(char const * const p_string) {
  int syscall_result;
  asm volatile("int $123" : "=a" (syscall_result) : "0" (1), "b" ((int)p_string));
}
