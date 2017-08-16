#pragma once

#include "cpu/types.h"

#define UNUSED(x) (void)(x)
#define NULL (0)

#define _KB (1024)
#define _MB (1048576)
#define _GB (1073741824)

void memory_copy(u8_t const * const p_source, u8_t * const p_destination, size_t const p_bytes_count);
void memory_set(u8_t * const p_destination, u8_t const p_value, size_t const p_bytes_count);
void int_to_ascii(int n, char str[]);
void reverse(char s[]);
size_t strlen(char s[]);
u32_t glue(u16_t const p_low, u16_t const p_high);
u32_t align_left(u32_t const p_a, u32_t const p_b);
u32_t align_right(u32_t const p_a, u32_t const p_b);
u32_t div_ceil(u32_t const p_a, u32_t const p_b);
void debug(char_t const * const p_string);
void debug(u32_t const p_integer);
