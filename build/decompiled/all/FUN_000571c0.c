
undefined4 __regparm3 FUN_000571c0(short *param_1,uint param_2,undefined4 param_3)

{
  undefined4 extraout_ECX;
  short *extraout_ECX_00;
  int extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 uVar1;
  uint extraout_EDX;
  int iVar2;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 4) = &PTR_FUN_000a277c;
    if (*(int *)(param_1 + 7) != 0) {
      for (iVar2 = 0; iVar2 < *param_1; iVar2 = iVar2 + 1) {
        thunk_FUN_0005c370(param_2,param_3);
        param_1 = extraout_ECX_00;
      }
      thunk_FUN_0005c370(param_2,param_3);
    }
    thunk_FUN_0005c370(param_2,param_3);
    *(undefined ***)(extraout_ECX_01 + 8) = &PTR_LAB_000a27a4;
    thunk_FUN_0005c370();
    uVar1 = extraout_ECX_02;
    if ((extraout_EDX & 2) != 0) {
      FUN_0005c370();
      uVar1 = extraout_ECX_03;
    }
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370(param_2,param_3);
    uVar1 = extraout_ECX;
  }
  return uVar1;
}

