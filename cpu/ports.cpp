#include "cpu/types.h"
#include "cpu/ports.h"

u8_t port_byte_in (u16_t const p_port) {
    u8_t result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (p_port));
    return result;
}

void port_byte_out (u16_t const p_port, u8_t const p_data) {
    __asm__("out %%al, %%dx" : : "a" (p_data), "d" (p_port));
}

u16_t port_word_in (u16_t const p_port) {
    u16_t result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (p_port));
    return result;
}

void port_word_out (u16_t const p_port, u16_t const p_data) {
    __asm__("out %%ax, %%dx" : : "a" (p_data), "d" (p_port));
}
