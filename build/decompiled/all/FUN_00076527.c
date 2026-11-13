
undefined8 __regparm3 FUN_00076527(undefined4 param_1,undefined4 param_2)

{
  int extraout_EDX;
  int iVar1;
  int iVar2;
  undefined8 uVar3;
  
  uVar3 = FUN_0007a635();
  iVar1 = (int)((ulonglong)uVar3 >> 0x20);
  iVar2 = (int)uVar3;
  if (iVar2 != -1) {
    if (((*(byte *)(iVar1 + 0xc) & 0x80) != 0) && ((*(byte *)(iVar1 + 0xd) & 0x10) != 0)) {
      FUN_0007744b();
      iVar1 = extraout_EDX;
    }
    if (*(int *)(iVar1 + 4) != 0) {
      if ((*(byte *)(iVar1 + 0xd) & 0x10) != 0) {
        return CONCAT44(param_2,iVar2 + *(int *)(iVar1 + 4));
      }
      iVar2 = iVar2 - *(int *)(iVar1 + 4);
    }
  }
  return CONCAT44(param_2,iVar2);
}

