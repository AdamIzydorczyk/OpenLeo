
int __regparm3 FUN_000565c0(int param_1,uint param_2)

{
  uint extraout_EDX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *(undefined ***)(param_1 + 8) = &PTR_LAB_000a2708;
  FUN_00056590();
  if ((extraout_EDX & 2) != 0) {
    FUN_0005c370();
  }
  return param_1;
}

