#include "cpu/ports.h"
#include "cpu/types.h"
#include "kernel/util.h"
#include "drivers/vga.h"

size_t get_cursor_offset();
void set_cursor_offset(size_t offset);
size_t print_char(char_t c, int col, int row, char_t attr);
size_t get_offset(size_t col, size_t row);
size_t get_offset_row(size_t offset);
size_t get_offset_col(size_t offset);

void kprint_at(str_t message, int col, int row) {
    /* Set cursor if col/row are negative */
    size_t offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    /* Loop through message and print it */
    size_t i = 0;
    while (message[i] != 0) {
        offset = print_char(message[i++], col, row, WHITE_ON_BLACK);
        /* Compute row/col for next iteration */
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

void kprint(str_t message) {
    kprint_at(message, -1, -1);
}


size_t print_char(char_t c, int col, int row, char_t attr) {
    u8_t *vidmem = (u8_t *) VIDEO_ADDRESS;
    if (!attr) attr = WHITE_ON_BLACK;

    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS) {
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-1] = RED_ON_WHITE;
        return get_offset(col, row);
    }

    size_t offset;
    if (col>=0 && row >= 0) offset = get_offset(col, row);
    else offset = get_cursor_offset();

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    } else {
        vidmem[offset] = c;
        vidmem[offset+1] = attr;
        offset += 2;
    }

    /* Check if the offset is over screen size and scroll */
    if (offset >= MAX_ROWS * MAX_COLS * 2) {
        size_t i;
        for (i = 1; i < MAX_ROWS; i++)
            memory_copy( (u8_t *)(get_offset(0, i) + VIDEO_ADDRESS),
                         (u8_t *)(get_offset(0, i-1) + VIDEO_ADDRESS),
                         MAX_COLS * 2);

        /* Blank last line */
        char_t *last_line = (char_t *)(get_offset(0, MAX_ROWS-1) + VIDEO_ADDRESS);
        for (i = 0; i < MAX_COLS * 2; i++) last_line[i] = 0;

        offset -= 2 * MAX_COLS;
    }

    set_cursor_offset(offset);
    return offset;
}

size_t get_cursor_offset() {
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    size_t offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2; /* Position * size of character cell */
}

void set_cursor_offset(size_t offset) {
    /* Similar to get_cursor_offset, but instead of reading we write data */
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (u8_t)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (u8_t)(offset & 0xff));
}

void clear_screen() {
    size_t screen_size = MAX_COLS * MAX_ROWS;
    size_t i;
    u8_t *screen = (u8_t*)VIDEO_ADDRESS;

    for (i = 0; i < screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(get_offset(0, 0));
}

void kprint(u32_t p_integer){
  char str[20];
  int_to_ascii(p_integer,str);
  kprint(str);
}

size_t get_offset(size_t col, size_t row) { return 2 * (row * MAX_COLS + col); }
size_t get_offset_row(size_t offset) { return offset / (2 * MAX_COLS); }
size_t get_offset_col(size_t offset) { return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; }
