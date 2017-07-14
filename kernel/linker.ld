OUTPUT_FORMAT("binary")

SECTIONS
{
  . = 0x1000;
  .text : { *(.text) }
  . = 0x8000;
  .data : { *(.data) }
  .bss : { *(.bss) }
}
