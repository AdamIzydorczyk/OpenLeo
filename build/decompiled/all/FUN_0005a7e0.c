
longlong __regparm3 FUN_0005a7e0(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  
  uVar2 = *(int *)(param_1 + 0x1c) - 1;
  if (-1 < (int)uVar2) {
    iVar1 = uVar2 * 0x10;
    do {
      piVar3 = (int *)(*(int *)(param_1 + 0x14) + iVar1);
      if (param_2 == *piVar3) {
        return CONCAT44(uVar2,piVar3[1]);
      }
      iVar1 = iVar1 + -0x10;
      uVar2 = uVar2 - 1;
    } while (-1 < iVar1);
  }
  return (ulonglong)uVar2 << 0x20;
}

