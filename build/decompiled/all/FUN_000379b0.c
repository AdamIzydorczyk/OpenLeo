
void __regparm3 FUN_000379b0(int *param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint uVar2;
  longdouble lVar3;
  
  if (599 < param_1[1]) {
    *(byte *)(param_2 + 7) = *(byte *)(param_2 + 7) | 1;
    iVar1 = (**(code **)(param_1[2] + 0xe4))();
    param_2 = extraout_ECX;
    if (iVar1 != 0) {
      iVar1 = FUN_00076bcc();
      if (iVar1 % 2 == 1) {
        iVar1 = 0x140;
      }
      else {
        iVar1 = 500;
      }
      param_1[1] = iVar1;
      (**(code **)(param_1[2] + 0xe8))();
      *(int *)((int)param_1 + 0x122) = *param_1;
      *(int *)((int)param_1 + 0x126) = param_1[1];
      *(byte *)(extraout_ECX_00 + 1) = *(byte *)(extraout_ECX_00 + 1) | 0x80;
      param_2 = extraout_ECX_00;
    }
  }
  if (0x23d < param_1[1]) {
    *(byte *)(param_2 + 6) = *(byte *)(param_2 + 6) | 0x80;
  }
  if (0x1e9 < param_1[1]) {
    *(byte *)(param_2 + 6) = *(byte *)(param_2 + 6) | 0x40;
  }
  iVar1 = param_1[1];
  if ((iVar1 < 0x1ea) || (0x23d < iVar1)) {
    *(int *)((int)param_1 + 0x122) = *param_1;
    *(int *)((int)param_1 + 0x126) = param_1[1];
  }
  else {
    lVar3 = (longdouble)FUN_00075d96(iVar1 - *(int *)((int)param_1 + 0x126));
    *param_1 = (int)ROUND(lVar3);
  }
  iVar1 = FUN_00076bcc();
  uVar2 = iVar1 % 3;
  if (uVar2 == 0) {
    *(byte *)(extraout_ECX_01 + 5) = *(byte *)(extraout_ECX_01 + 5) | 2;
  }
  else if (uVar2 < 2) {
    *(byte *)(extraout_ECX_01 + 5) = *(byte *)(extraout_ECX_01 + 5) | 4;
  }
  else if (uVar2 == 2) {
    *(byte *)(extraout_ECX_01 + 5) = *(byte *)(extraout_ECX_01 + 5) | 8;
  }
  FUN_00037880();
  return;
}

