#include "api.h"

void print(const char *p_string) {
  int syscall_result;
  asm volatile("int $123" : "=a" (syscall_result) : "0" (0), "b" ((int)p_string));
}
