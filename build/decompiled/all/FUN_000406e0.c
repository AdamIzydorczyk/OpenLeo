
undefined4 __regparm3 FUN_000406e0(undefined4 *param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < param_2) {
    do {
      bVar1 = in((short)*param_1 + 0xc);
      if ((bVar1 & 0x80) == 0) {
        return 1;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < param_2);
  }
  return 0;
}

