
undefined4 * __regparm3 FUN_0005ac10(undefined4 *param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_ECX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *param_1 = &PTR_LAB_000a2a6c;
  iVar1 = FUN_00057a00(param_3);
  if ((extraout_ECX & 2) != 0) {
    FUN_0005c370();
  }
  return (undefined4 *)(iVar1 + -4);
}

