#include "cpu/ports.h"
#include "cpu/types.h"
#include "kernel/util.h"
#include "drivers/vga.h"

#define VIDEO_ADDRESS (0xb8000)
#define MAX_ROWS 25
#define MAX_COLS 80
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

u8_t *video_memory = reinterpret_cast<u8_t *>(VIDEO_ADDRESS);
void set_char_at(size_t p_x, size_t p_y, char_t p_char) {
  if (p_y < MAX_ROWS && p_x < MAX_COLS)
    video_memory[2*(p_y * MAX_COLS + p_x)] = p_char;
}
void set_color_at(size_t p_x, size_t p_y, u8_t p_color) {
  if (p_y < MAX_ROWS && p_x < MAX_COLS)
    video_memory[2*(p_y * MAX_COLS + p_x) + 1] = p_color;
}
void set_cursor_at(size_t p_x, size_t p_y) {
  size_t offset = p_y * MAX_COLS + p_x;
  port_byte_out(REG_SCREEN_CTRL, 14);
  port_byte_out(REG_SCREEN_DATA, (u8_t)(offset >> 8));
  port_byte_out(REG_SCREEN_CTRL, 15);
  port_byte_out(REG_SCREEN_DATA, (u8_t)(offset & 0xff));
}
