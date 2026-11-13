
int __regparm3 FUN_00021670(int param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_ECX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *(undefined ***)(param_1 + 0x10) = &PTR_FUN_0009763c;
  FUN_00021b20(param_3);
  iVar1 = FUN_00021b70();
  if ((extraout_ECX & 2) != 0) {
    FUN_0005c370();
  }
  return iVar1;
}

