OUTPUT_FORMAT("binary")

SECTIONS
{
  . = 0x00001600;
  .text : { * (.text); }
}
