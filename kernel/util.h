#ifndef __KERNEL__UTIL__
#define __KERNEL__UTIL__

#include "cpu/types.h"

#define UNUSED(x) (void)(x)
#define NULL (0)

void memory_copy(u8_t *source, u8_t *dest, int nbytes);
void memory_set(u8_t *dest, u8_t val, u32_t len);
void int_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
u32_t glue(u16_t const p_low, u16_t const p_high);

#endif
