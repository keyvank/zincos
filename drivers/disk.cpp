#include "drivers/disk.h"
#include "kernel/util.h"
#include "cpu/types.h"

void read_sectors(u32_t const p_lba, u32_t const p_count, u8_t * const p_buffer) {
  for(u32_t i = 0; i < p_count; i++)
    read_sector(p_lba + i, p_buffer + 512 * i);
}
