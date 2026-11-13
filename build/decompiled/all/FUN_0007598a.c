
void __regparm3 FUN_0007598a(int param_1)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  undefined4 *extraout_ECX_00;
  undefined4 *extraout_ECX_01;
  
  if ((*(byte *)(param_1 + 0x24) & 2) == 0) {
    FUN_000756f8();
    iVar1 = FUN_000794e0();
    extraout_ECX_01[3] = iVar1;
    *(undefined4 *)(iVar1 + 0x18) = extraout_ECX_01[6];
    *(undefined4 *)(iVar1 + 0x14) = *extraout_ECX_01;
  }
  else {
    iVar1 = FUN_00079648();
    if (iVar1 != *(int *)(extraout_ECX + 0xc)) {
      FUN_000756f8();
    }
    FUN_00079657();
    iVar2 = FUN_000754b9();
    if (iVar2 != 0) {
      *(undefined4 *)(iVar1 + 0x18) = extraout_ECX_00[6];
      *(undefined4 *)(iVar1 + 0x14) = *extraout_ECX_00;
      return;
    }
  }
  return;
}

