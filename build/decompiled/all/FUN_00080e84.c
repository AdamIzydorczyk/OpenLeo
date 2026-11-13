
longlong __regparm3 FUN_00080e84(int param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  int extraout_ECX;
  
  if (*(int *)(param_1 + 0x20) == *(int *)(param_1 + 0x18)) {
    if (*(int *)(param_1 + 0x10) != *(int *)(param_1 + 0x14)) {
      if (((*(byte *)(param_1 + 0x30) & 0x80) != 0) &&
         (uVar2 = *(uint *)(param_1 + 0x10), uVar2 != 0)) {
        do {
          uVar2 = uVar2 - 1;
        } while (*(uint *)(param_1 + 0x14) <= uVar2);
      }
      iVar1 = FUN_0007a5bd(param_2,param_3);
      if (iVar1 < 0) {
        return CONCAT44(param_2,0xffffffff);
      }
      *(undefined4 *)(extraout_ECX + 0xc) = 0;
      *(undefined4 *)(extraout_ECX + 0x14) = 0;
      *(undefined4 *)(extraout_ECX + 0x10) = 0;
    }
  }
  else {
    iVar1 = (**(code **)(*(int *)(param_1 + 0x28) + 0xc))();
    if (iVar1 == -1) {
      return CONCAT44(param_2,0xffffffff);
    }
  }
  return (ulonglong)param_2 << 0x20;
}

