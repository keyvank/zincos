#pragma once

#include "cpu/types.h"

#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

void set_char_at(size_t p_row, size_t p_column, char_t p_char);
void set_color_at(size_t p_row, size_t p_column, u8_t p_color);
void set_cursor_at(size_t p_row, size_t p_column);
