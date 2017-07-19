#ifndef __CPU__TYPES__
#define __CPU__TYPES__

/* Instead of using 'chars' to allocate non-character bytes,
 * we will use these new type with no semantic meaning */
typedef unsigned int u32_t;
typedef int s32_t;
typedef unsigned short u16_t;
typedef short s16_t;
typedef unsigned char u8_t;
typedef char s8_t;
typedef void *addr_t;
typedef u32_t size_t;
typedef char char_t;
typedef char_t const *str_t;
typedef u8_t bool_t;

#define true (1)
#define false (0)

#define low_16(address) (u16_t)((address) & 0xFFFF)
#define high_16(address) (u16_t)(((address) >> 16) & 0xFFFF)

#endif
