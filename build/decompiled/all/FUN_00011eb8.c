
longlong __regparm3 FUN_00011eb8(undefined4 param_1,uint param_2)

{
  byte bVar1;
  short sVar2;
  
  sVar2 = 0x7fff;
  do {
    sVar2 = sVar2 + -1;
    if (sVar2 == -1) {
      return (ulonglong)param_2 << 0x20;
    }
    bVar1 = in(DAT_000ae644 + 0xc);
  } while ((bVar1 & 0x80) != 0);
  return CONCAT44(param_2,1);
}

