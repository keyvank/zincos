#include "cpu/timer.h"
#include "drivers/vga.h"
#include "kernel/util.h"
#include "cpu/isr.h"
#include "cpu/ports.h"

void init_timer(u32_t const p_frequency, isr_t const p_handler) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, p_handler);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32_t divisor = 1193180 / p_frequency;
    u8_t low  = (u8_t)(divisor & 0xFF);
    u8_t high = (u8_t)( (divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}
