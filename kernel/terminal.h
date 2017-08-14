#pragma once

#include "cpu/types.h"
#include "libcpp/string.h"
class kernel;
class process;

class terminal {
  friend class kernel;
private:
  kernel &m_kernel;
  process *m_process;
  bool m_active;
  u8_t *m_text;
  u8_t *m_colors;
  size_t m_cursor_x;
  size_t m_cursor_y;
  string *m_input_buffer;
  void backspace();
  void keyboard_event(u8_t const p_scancode, bool const p_is_up, bool const p_is_ctrl, bool const p_is_alt, bool const p_is_shift);
public:

  terminal(kernel &p_kernel);
  terminal(terminal const &) = delete;
  terminal(terminal &&) = delete;
  terminal &operator=(terminal const &) = delete;
  terminal &operator=(terminal &&) = delete;
  ~terminal();

  void set_active();
  void set_inactive();
  void refresh();
  void clear();
  void write_char_at(size_t p_row, size_t p_column, char_t const p_char);
  void write(char_t const * const p_string);
  void write(size_t const p_integer);
  void write_line(char_t const * const p_string);
};
