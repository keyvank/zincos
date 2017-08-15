#include "api.h"

void exit(unsigned int const p_exit_code) {
  int syscall_result;
  asm volatile("int $123" : "=a" (syscall_result) : "0" (0), "b" (p_exit_code));
}

void print(char const * const p_string) {
  int syscall_result;
  asm volatile("int $123" : "=a" (syscall_result) : "0" (1), "b" ((int)p_string));
}

char getch() {
  int syscall_result;
  volatile char p_buffer = 123;
  asm volatile("int $123" : "=a" (syscall_result) : "0" (2), "b" ((int)&p_buffer), "c" (1));
  return p_buffer;
}

void read_line(char * const p_buffer) {
  char *curr = p_buffer;
  char ch;
  while((ch=getch())!='\n')
    *(curr++) = ch;
  *curr = '\0';
}

void *alloc_pages(unsigned long int const p_count) {
  int syscall_result;
  volatile unsigned long int address = 0;
  asm volatile("int $123" : "=a" (syscall_result) : "0" (3), "b" ((int)p_count), "c" ((int)&address));
  return reinterpret_cast<void *>(address);
}
