#pragma once

#define UNUSED(x) (void)(x)

void exit(unsigned int const p_exit_code);
void print(const char *p_string);
void read_line(char * const p_buffer);
void thread(void (*p_entry)());
char getch();
void *alloc_pages(unsigned long int const p_count);
