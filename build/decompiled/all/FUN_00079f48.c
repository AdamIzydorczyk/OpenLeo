
int * __regparm3 FUN_00079f48(int *param_1,byte param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar2;
  undefined4 local_14;
  undefined *local_10;
  undefined4 local_c;
  undefined4 uStack_8;
  
  uStack_8 = param_3;
  if ((param_2 & 4) == 0) {
    local_10 = &DAT_000a6a44;
    local_14 = DAT_000cf4f4;
    DAT_000cf4f4 = &local_14;
    *(int *)(*(int *)(*param_1 + 4) + -4 + (int)param_1) = *(int *)(*param_1 + 4);
    param_1[0x10] = (int)&PTR_FUN_000a69f8;
    *(undefined ***)((int)param_1 + *(int *)(*param_1 + 4) + 0x28) = &PTR_LAB_000a69f4;
    local_c = 1;
    iVar1 = FUN_0007f7b4(param_1,param_2);
    param_1 = (int *)(iVar1 + -4);
    uVar2 = extraout_ECX;
    if ((extraout_ECX & 1) == 0) {
      local_c = 0;
      iVar1 = FUN_00079f10();
      param_1 = (int *)(iVar1 + -0x48);
      uVar2 = extraout_ECX_00;
    }
    if ((uVar2 & 2) != 0) {
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

