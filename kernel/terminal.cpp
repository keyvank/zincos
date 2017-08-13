#include "kernel/terminal.h"
#include "drivers/vga.h"

terminal::terminal(kernel &p_kernel) : m_kernel(p_kernel) {

}

void terminal::write(char_t const * const p_string) {
  kprint(p_string);
}

void terminal::write_line(char_t const * const p_string) {
  kprint(p_string);
  kprint("\n");
}

terminal::~terminal() {

}
