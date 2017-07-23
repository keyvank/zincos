#include "kernel/util.h"
#include "drivers/vga.h"

void memory_copy(u8_t const * const p_source, u8_t * const p_destination, size_t const p_bytes_count) {
    for(size_t i = 0; i < p_bytes_count; i++)
        *(p_destination + i) = *(p_source + i);
}

void memory_set(u8_t * const p_destination, u8_t const p_value, size_t const p_bytes_count) {
    for(size_t i = 0; i < p_bytes_count; i++)
      *(p_destination + i) = p_value;
}

/**
 * K&R implementation
 */
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

/* K&R */
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
size_t strlen(char s[]) {
    size_t i = 0;
    while (s[i] != '\0') ++i;
    return i;
}


u32_t glue(u16_t const p_low, u16_t const p_high) {
	return (u32_t)p_low + (((u32_t)p_high)<<16);
}

u32_t div_ceil(u32_t const p_a, u32_t const p_b) {
  return p_a / p_b + (p_a % p_b != 0);
}

u32_t align_right(u32_t const p_a, u32_t const p_b) {
  return div_ceil(p_a, p_b) * p_b;
}
u32_t align_left(u32_t const p_a, u32_t const p_b) {
  return (p_a / p_b) * p_b;
}
