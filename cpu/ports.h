#ifndef __DRIVERS__PORTS__
#define __DRIVERS__PORTS__

#include "cpu/types.h"

u8_t port_byte_in (u16_t const p_port);
void port_byte_out (u16_t const p_port, u8_t const p_data);
u16_t port_word_in (u16_t const p_port);
void port_word_out (u16_t const p_port, u16_t const p_data);

#endif
