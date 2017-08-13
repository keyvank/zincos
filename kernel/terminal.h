#pragma once

#include "cpu/types.h"
class kernel;

class terminal {
private:
  kernel &m_kernel;
  u8_t *m_screen;
  u8_t *m_input_buffer;
public:

  terminal(kernel &p_kernel);
  terminal(terminal const &) = delete;
  terminal(terminal &&) = delete;
  terminal &operator=(terminal const &) = delete;
  terminal &operator=(terminal &&) = delete;
  ~terminal();

  void write(char_t const * const p_string);
  void write_line(char_t const * const p_string);
};
