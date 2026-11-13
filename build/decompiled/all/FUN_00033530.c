
int __regparm3 FUN_00033530(int param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_ECX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *(undefined ***)(param_1 + 0x26) = &PTR_LAB_0009d590;
  FUN_00044ab0(param_3);
  FUN_00049ba0();
  FUN_00057a00();
  FUN_00049af0();
  iVar1 = FUN_00057a00();
  if ((extraout_ECX & 2) != 0) {
    FUN_0005c370();
  }
  return iVar1 + -0x11;
}

