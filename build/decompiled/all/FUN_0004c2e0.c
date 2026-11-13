
int __regparm3 FUN_0004c2e0(int param_1,uint param_2)

{
  uint extraout_EDX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *(undefined ***)(param_1 + 0x10) = &PTR_FUN_000a1338;
  FUN_0004c290();
  if ((extraout_EDX & 2) != 0) {
    FUN_0005c370();
  }
  return param_1;
}

