
int __regparm3 FUN_0002d8f0(int param_1,uint param_2)

{
  uint extraout_EDX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *(undefined **)(param_1 + 0x10) = &DAT_0009b4bc;
  FUN_0002d8a0();
  if ((extraout_EDX & 2) != 0) {
    FUN_0005c370();
  }
  return param_1;
}

