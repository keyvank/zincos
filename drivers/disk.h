#include "cpu/types.h"

extern "C" void read_sector(u32_t const p_lba, u8_t * const p_buffer);
void read_sectors(u32_t const p_lba, u32_t const p_count, u8_t * const p_buffer);
