
void __regparm3 FUN_00057600(int param_1,int param_2)

{
  uint uVar1;
  int unaff_EBX;
  int iVar2;
  
  uVar1 = param_2 - (*(int *)(param_1 + 0x10) + 0xa0);
  iVar2 = *(int *)(param_1 + 0x2c) + (uVar1 - *(int *)(param_1 + 0x2c) / *(int *)(param_1 + 0x34));
  *(int *)(param_1 + 0x2c) = iVar2;
  *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + iVar2 / *(int *)(param_1 + 0x3c);
  if ((3 < (int)((uVar1 ^ (int)uVar1 >> 0x1f) - ((int)uVar1 >> 0x1f))) &&
     (uVar1 = *(uint *)(param_1 + 0x2c), uVar1 != 0)) {
    *(int *)(param_1 + 0x10) =
         *(int *)(param_1 + 0x10) +
         *(int *)(param_1 + 0x2c) / (int)((uVar1 ^ (int)uVar1 >> 0x1f) - ((int)uVar1 >> 0x1f));
  }
  uVar1 = unaff_EBX - (*(int *)(param_1 + 0x14) + 100);
  iVar2 = *(int *)(param_1 + 0x30) + (uVar1 - *(int *)(param_1 + 0x30) / *(int *)(param_1 + 0x38));
  *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + iVar2 / *(int *)(param_1 + 0x40);
  *(int *)(param_1 + 0x30) = iVar2;
  if ((3 < (int)((uVar1 ^ (int)uVar1 >> 0x1f) - ((int)uVar1 >> 0x1f))) &&
     (uVar1 = *(uint *)(param_1 + 0x30), uVar1 != 0)) {
    *(int *)(param_1 + 0x14) =
         *(int *)(param_1 + 0x14) +
         *(int *)(param_1 + 0x30) / (int)((uVar1 ^ (int)uVar1 >> 0x1f) - ((int)uVar1 >> 0x1f));
  }
  FUN_00057590();
  return;
}

