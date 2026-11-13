
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_00037d50(int *param_1,int param_2)

{
  int iVar1;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int extraout_ECX_01;
  uint uVar2;
  longdouble lVar3;
  
  if (600 < param_1[1]) {
    *(byte *)(param_2 + 7) = *(byte *)(param_2 + 7) | 1;
    iVar1 = (**(code **)(param_1[2] + 0xe4))();
    param_1 = extraout_ECX;
    if (iVar1 != 0) {
      FUN_00037ba0();
      *(byte *)(param_2 + 1) = *(byte *)(param_2 + 1) | 0x80;
      param_1 = extraout_ECX_00;
    }
  }
  iVar1 = *param_1;
  lVar3 = (longdouble)FUN_00037d00();
  if ((longdouble)_DAT_000911ec < (longdouble)iVar1 - lVar3) {
    *(byte *)(param_2 + 6) = *(byte *)(param_2 + 6) | 0x80;
  }
  iVar1 = *(int *)(extraout_ECX_01 + 4);
  lVar3 = (longdouble)FUN_00037ce0();
  if ((longdouble)_DAT_000911f4 <= (longdouble)iVar1 - lVar3) {
    *(byte *)(param_2 + 6) = *(byte *)(param_2 + 6) | 0x40;
  }
  iVar1 = FUN_00076bcc();
  uVar2 = iVar1 % 3;
  if (uVar2 == 0) {
    *(byte *)(param_2 + 5) = *(byte *)(param_2 + 5) | 2;
  }
  else if (uVar2 < 2) {
    *(byte *)(param_2 + 5) = *(byte *)(param_2 + 5) | 4;
  }
  else if (uVar2 == 2) {
    *(byte *)(param_2 + 5) = *(byte *)(param_2 + 5) | 8;
  }
  FUN_00037880();
  return;
}

