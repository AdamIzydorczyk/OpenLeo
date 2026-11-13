
int * __regparm3 FUN_00079e88(int *param_1,byte param_2)

{
  int iVar1;
  undefined4 local_1c;
  undefined *local_18;
  undefined4 local_14;
  int *local_10;
  byte local_c;
  
  local_18 = &DAT_000a6a04;
  local_14 = 0;
  local_1c = DAT_000cf4f4;
  if ((param_2 & 1) == 0) {
    DAT_000cf4f4 = &local_1c;
    *param_1 = (int)&DAT_000a6a28;
    local_10 = param_1;
    local_c = param_2;
    iVar1 = FUN_00079dd4();
    param_1 = (int *)(iVar1 + -0xc);
  }
  *(int *)(*(int *)(*param_1 + 4) + -4 + (int)param_1) = *(int *)(*param_1 + 4);
  param_1[1] = (int)&PTR_FUN_000a6a24;
  *(undefined ***)((int)param_1 + *(int *)(*param_1 + 4) + 0x28) = &PTR_LAB_000a6a20;
  DAT_000cf4f4 = (undefined4 *)local_1c;
  return param_1;
}

