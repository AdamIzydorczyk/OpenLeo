
int * __regparm3 FUN_0007a004(int *param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_ECX;
  undefined4 local_14;
  undefined *local_10;
  undefined4 local_c;
  undefined4 uStack_8;
  
  uStack_8 = param_3;
  if ((param_2 & 4) == 0) {
    local_10 = &DAT_000a6a88;
    local_c = 1;
    local_14 = DAT_000cf4f4;
    DAT_000cf4f4 = &local_14;
    *(int *)(*(int *)(*param_1 + 4) + -4 + (int)param_1) = *(int *)(*param_1 + 4);
    param_1[1] = (int)&PTR_FUN_000a6a24;
    *(undefined ***)((int)param_1 + *(int *)(*param_1 + 4) + 0x28) = &PTR_LAB_000a6a20;
    if ((param_2 & 1) == 0) {
      local_c = 0;
      iVar1 = FUN_00079f10(param_1,(char)param_2);
      param_1 = (int *)(iVar1 + -0xc);
      param_2 = extraout_ECX;
    }
    if ((param_2 & 2) != 0) {
      FUN_0005c370();
    }
    DAT_000cf4f4 = (undefined4 *)local_14;
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
  }
  return param_1;
}

