
int __regparm3 FUN_0005e310(int param_1,uint param_2)

{
  uint extraout_ECX;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 0xac) = &PTR_FUN_000a2c70;
    if (DAT_000cef40 != 0) {
      (**(code **)(*(int *)(DAT_000cef40 + 0x3e6) + 4))();
    }
    DAT_000cef40 = 0;
    FUN_0005dcc0();
    if ((extraout_ECX & 2) != 0) {
      FUN_0005c370();
    }
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
  }
  return param_1;
}

