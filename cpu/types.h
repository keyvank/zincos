#pragma once

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

typedef struct {
   u32_t esp;
   u32_t ebp;
   u32_t eip;
   u32_t edi;
   u32_t esi;
   u32_t eax;
   u32_t ebx;
   u32_t ecx;
   u32_t edx;
   u32_t flags;
} __attribute__((packed)) cpu_state_t;

#define true (1)
#define false (0)

#define low_16(address) (u16_t)((address) & 0xFFFF)
#define high_16(address) (u16_t)(((address) >> 16) & 0xFFFF)
