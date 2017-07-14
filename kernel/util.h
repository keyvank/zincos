#ifndef __KERNEL__UTIL__
#define __KERNEL__UTIL__

#include "cpu/types.h"

void memory_copy(char *source, char *dest, int nbytes);
void memory_set(u8 *dest, u8 val, u32 len);
void int_to_ascii(int n, char str[]);

#endif
