#include "drivers/keyboard.h"
#include "kernel/util.h"
#include "cpu/ports.h"
#include "cpu/isr.h"
#include "drivers/vga.h"

void print_letter(u8_t scancode);
void init_keyboard();

void init_keyboard(isr_t const p_handler) {
   register_interrupt_handler(IRQ1, p_handler);
}


char to_char(u8_t const p_scancode, bool const p_shift) {

  switch (p_scancode) {
    case 0x2:
        return (p_shift?'!':'1');
    case 0x3:
        return (p_shift?'@':'2');

    case 0x4:
        return (p_shift?'#':'3');

    case 0x5:
        return (p_shift?'$':'4');

    case 0x6:
        return (p_shift?'%':'5');

    case 0x7:
        return (p_shift?'^':'6');

    case 0x8:
        return (p_shift?'&':'7');

    case 0x9:
        return (p_shift?'*':'8');

    case 0x0A:
        return (p_shift?'(':'9');

    case 0x0B:
        return (p_shift?')':'0');

    case 0x0C:
        return (p_shift?'_':'-');

    case 0x0D:
        return (p_shift?'+':'=');

    case 0x10:
        return (p_shift?'Q':'q');

    case 0x11:
        return (p_shift?'W':'w');

    case 0x12:
        return (p_shift?'E':'e');

    case 0x13:
        return (p_shift?'R':'r');

    case 0x14:
        return (p_shift?'T':'t');

    case 0x15:
        return (p_shift?'Y':'y');

    case 0x16:
        return (p_shift?'U':'u');

    case 0x17:
        return (p_shift?'I':'i');

    case 0x18:
        return (p_shift?'O':'o');

    case 0x19:
        return (p_shift?'P':'p');

    case 0x1A:
      return (p_shift?'{':'[');

    case 0x1B:
      return (p_shift?'}':']');

    case 0x1E:
      return (p_shift?'A':'a');

    case 0x1F:
      return (p_shift?'S':'s');

    case 0x20:
        return (p_shift?'D':'d');

    case 0x21:
        return (p_shift?'F':'f');

    case 0x22:
        return (p_shift?'G':'g');

    case 0x23:
        return (p_shift?'H':'h');

    case 0x24:
        return (p_shift?'J':'j');

    case 0x25:
        return (p_shift?'K':'k');

    case 0x26:
        return (p_shift?'L':'l');

    case 0x27:
        return (p_shift?':':';');;

    case 0x28:
        return (p_shift?'\"':'\'');

    case 0x29:
        return (p_shift?'~':'`');;

    case 0x2B:
      return (p_shift?'|':'\\');

    case 0x2C:
      return (p_shift?'Z':'z');

    case 0x2D:
      return (p_shift?'X':'x');

    case 0x2E:
      return (p_shift?'C':'c');

    case 0x2F:
      return (p_shift?'V':'v');

    case 0x30:
        return (p_shift?'B':'b');

    case 0x31:
        return (p_shift?'N':'n');

    case 0x32:
        return (p_shift?'M':'m');

    case 0x33:
        return (p_shift?'<':',');

    case 0x34:
        return (p_shift?'>':'.');

    case 0x35:
        return (p_shift?'?':'/');

    case 0x39:
        return ' ';

    default:
        return 0;
  }
}

bool is_up(u8_t const p_scancode) {
  return p_scancode > 0x39;
}
