
int * __regparm3 FUN_00079ddc(undefined4 *param_1,byte param_2)

{
  int *piVar1;
  int iVar2;
  bool bVar3;
  undefined4 local_10;
  undefined *local_c;
  uint local_8;
  
  local_c = &DAT_000a69c4;
  local_8 = 0;
  local_10 = DAT_000cf4f4;
  DAT_000cf4f4 = &local_10;
  bVar3 = (param_2 & 1) == 0;
  if (bVar3) {
    *param_1 = &DAT_000a69fc;
    FUN_00079dd4(param_1,param_2);
  }
  local_8 = (uint)bVar3;
  iVar2 = FUN_0007f704();
  piVar1 = (int *)(iVar2 + -4);
  local_8 = 2;
  *(int *)(*(int *)(*piVar1 + 4) + -4 + (int)piVar1) = *(int *)(*piVar1 + 4);
  *(undefined ***)(iVar2 + 0x3c) = &PTR_FUN_000a69f8;
  *(undefined ***)((int)piVar1 + *(int *)(*piVar1 + 4) + 0x28) = &PTR_LAB_000a69f4;
  FUN_0007f768();
  DAT_000cf4f4 = (undefined4 *)local_10;
  return piVar1;
}

