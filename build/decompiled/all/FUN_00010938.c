
byte FUN_00010938(void)

{
  DAT_000ade99 = DAT_000ade99 & 0xfb;
  out(DAT_000ade94,DAT_000ade99);
  return DAT_000ade99;
}

