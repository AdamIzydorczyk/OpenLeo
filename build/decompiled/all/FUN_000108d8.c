
byte FUN_000108d8(void)

{
  DAT_000ade99 = DAT_000ade99 & 0xfd;
  out(DAT_000ade94,DAT_000ade99);
  return DAT_000ade99;
}

