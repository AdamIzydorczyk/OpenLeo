
int * __regparm3 FUN_00075dc0(undefined4 *param_1,byte param_2)

{
  int *piVar1;
  int iVar2;
  bool bVar3;
  undefined4 local_1c;
  undefined *local_18;
  uint local_14;
  undefined4 *local_10;
  byte local_c;
  
  local_18 = &DAT_000a67f4;
  local_14 = 0;
  local_1c = DAT_000cf4f4;
  bVar3 = (param_2 & 1) == 0;
  DAT_000cf4f4 = &local_1c;
  local_10 = param_1;
  local_c = param_2;
  if (bVar3) {
    DAT_000cf4f4 = &local_1c;
    *param_1 = &DAT_000a6850;
    param_1[0x11] = &DAT_000a6848;
    FUN_00079dd4();
  }
  local_14 = (uint)bVar3;
  FUN_00079ddc();
  local_14 = 2;
  iVar2 = FUN_00079e88();
  piVar1 = (int *)(iVar2 + -0x44);
  *(int *)(*(int *)(*piVar1 + 4) + -4 + (int)piVar1) = *(int *)(*piVar1 + 4);
  *(undefined ***)(iVar2 + -4) = &PTR_FUN_000a6844;
  *(undefined ***)(iVar2 + 4) = &PTR_LAB_000a6840;
  *(undefined ***)((int)piVar1 + *(int *)(*piVar1 + 4) + 0x28) = &PTR_LAB_000a683c;
  DAT_000cf4f4 = (undefined4 *)local_1c;
  return piVar1;
}

