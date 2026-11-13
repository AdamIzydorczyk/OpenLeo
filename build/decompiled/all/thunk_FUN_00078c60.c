
undefined4 __regparm3 thunk_FUN_00078c60(int param_1)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 extraout_EDX;
  bool bVar3;
  
  uVar1 = *(uint *)(param_1 + 0xc);
  *(byte *)(param_1 + 0xc) = *(byte *)(param_1 + 0xc) & 0xcf;
  if (*(int *)(param_1 + 8) == 0) {
    FUN_0007a448();
  }
  bVar3 = (*(byte *)(param_1 + 0xd) & 4) != 0;
  if (bVar3) {
    *(byte *)(param_1 + 0xd) = *(byte *)(param_1 + 0xd) & 0xfa | 1;
  }
  uVar2 = FUN_0007ab85();
  if (bVar3) {
    *(byte *)(param_1 + 0xd) = *(byte *)(param_1 + 0xd) & 0xfa | 4;
    FUN_0007a4e4();
    uVar2 = extraout_EDX;
  }
  if ((*(byte *)(param_1 + 0xc) & 0x20) != 0) {
    uVar2 = 0xffffffff;
  }
  *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) | uVar1 & 0x30;
  return uVar2;
}

