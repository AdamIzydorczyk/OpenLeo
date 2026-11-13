
longlong __regparm3 FUN_0007f975(uint param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  undefined8 uVar2;
  
  if (DAT_000ad504 <= param_1) {
    return (ulonglong)param_2 << 0x20;
  }
  if ((int)param_1 < 6) {
    iVar1 = param_1 * 4;
    if ((PTR_DAT_000ad558[iVar1 + 1] & 0x40) == 0) {
      PTR_DAT_000ad558[iVar1 + 1] = PTR_DAT_000ad558[iVar1 + 1] | 0x40;
      uVar2 = FUN_0007f94e(param_2,param_3);
      param_1 = (uint)((ulonglong)uVar2 >> 0x20);
      if ((int)uVar2 != 0) {
        PTR_DAT_000ad558[iVar1 + 1] = PTR_DAT_000ad558[iVar1 + 1] | 0x20;
      }
    }
  }
  return CONCAT44(param_2,*(undefined4 *)(PTR_DAT_000ad558 + param_1 * 4));
}

