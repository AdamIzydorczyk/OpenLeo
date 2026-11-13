
int __regparm3 FUN_00052960(int param_1,uint param_2)

{
  uint extraout_ECX;
  undefined4 extraout_EDX;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_FUN_000a2550;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_000a2548;
    if (*(int *)(param_1 + 0x30) != 0) {
      FUN_000554b0();
    }
    while (*(int *)(param_1 + 0x10) != 0) {
      FUN_0005c370();
      *(undefined4 *)(param_1 + 0x10) = extraout_EDX;
    }
    FUN_00052c40();
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

