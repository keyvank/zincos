OUTPUT_FORMAT("binary")

SECTIONS
{
  . = 0x00001000;
  .text : { * (.text); }
}
