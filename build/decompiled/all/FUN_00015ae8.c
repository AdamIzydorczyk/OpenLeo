
ushort __regparm3 FUN_00015ae8(ushort param_1)

{
  if ((DAT_000ae672 & 2) != 0) {
    param_1 = param_1 >> 1;
  }
  if ((DAT_000ae672 & 1) != 0) {
    param_1 = param_1 >> 1;
  }
  return param_1;
}

