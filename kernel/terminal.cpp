#include "kernel/kernel.h"
#include "kernel/terminal.h"
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "libcpp/string.h"
#include "kernel/util.h"
#include "cpu/asmutil.h"

terminal::terminal(kernel &p_kernel) :
  m_kernel(p_kernel),
  m_active(false),
  m_text(new u8_t[80 * 25]),
  m_colors(new u8_t[80 * 25]),
  m_cursor_x(0), m_cursor_y(0),
  m_input_buffer() {

  this->clear();
}

void terminal::refresh() {
  if(this->m_active) {
    for(size_t i = 0; i < 25; i++)
      for(size_t j = 0; j < 80; j++) {
        set_char_at(j, i, this->m_text[i * 80 + j]);
        set_color_at(j, i, this->m_colors[i * 80 + j]);
      }
    set_cursor_at(m_cursor_x, m_cursor_y);
  }
}

void terminal::write(char_t const * const p_string) {
  char_t const * current = p_string;
  while(*current) {
    if(*current == '\n') {
      this->m_cursor_y++;
      this->m_cursor_x=0;
    } else {
      this->write_char_at(this->m_cursor_x, this->m_cursor_y, *current);
      this->m_cursor_x++;
      if(this->m_cursor_x == 80) {
        this->m_cursor_x = 0;
        this->m_cursor_y++;
      }
    }
    if(this->m_cursor_y == 24) {
      memory_copy(this->m_text + 80, this->m_text, 80 * 24);
      memory_copy(this->m_colors + 80, this->m_colors, 80 * 24);
      this->m_cursor_y--;
      refresh();
    }
    if(this->m_active)
      set_cursor_at(this->m_cursor_x, this->m_cursor_y);
    current++;
  }
}

void terminal::write(size_t const p_integer) {
  char_t buff[16];
  int_to_ascii(p_integer,buff);
  this->write(buff);
}

void terminal::clear() {
  memory_set(this->m_text, ' ', 80 * 25);
  memory_set(this->m_colors, WHITE_ON_BLACK, 80 * 25);
  if(this->m_active) refresh();
}

void terminal::write_char_at(size_t const p_x, size_t const p_y, char_t const p_char) {
  this->m_text[p_y * 80 + p_x] = p_char;
  if(this->m_active)
    set_char_at(this->m_cursor_x, this->m_cursor_y, p_char);
}

void terminal::write_line(char_t const * const p_string) {
  this->write(p_string);
}

void terminal::set_active() {
  this->m_active = true;
  refresh();
}

void terminal::set_inactive() {
  this->m_active = false;
}

terminal::~terminal() {
  delete[] this->m_text;
  delete[] this->m_colors;
}


void terminal::backspace() {
  if(m_cursor_x == 0) {
    m_cursor_y--;
    m_cursor_x = 79;
  } else m_cursor_x--;
  write_char_at(this->m_cursor_x, this->m_cursor_y, ' ');
  set_cursor_at(this->m_cursor_x, this->m_cursor_y);
}

void terminal::keyboard_event(u8_t const p_scancode, bool p_is_up, bool const p_is_ctrl, bool const p_is_alt, bool const p_is_shift) {
  UNUSED(p_is_ctrl);
  UNUSED(p_is_alt);
  if(this->m_process->m_sys_read_count == 0) // Block if program hasn't requested to read.
    return;
  char ch = to_char(p_scancode, p_is_shift);
  if(!p_is_up) {
    if(p_scancode == KEY_BACKSPACE) {
      if(m_input_buffer.get_length() > 0) {
        m_input_buffer.pop_char();
        this->backspace();
      }
    }
    else if(p_scancode == KEY_ENTER) {
      this->write("\n");
      while(this->m_input_buffer.get_length() > 0)
        this->m_process->m_input_buffer.put_char(this->m_input_buffer.peek_char());
      this->m_process->m_input_buffer.put_char('\n');
      this->m_process->flush_input_buffer();
    }
    if(ch) {
      this->m_input_buffer.put_char(ch);
      char str[2]={ch,'\0'};
      this->write(str);
    }
  }
}
