
int __regparm3 FUN_00050180(int param_1,uint param_2)

{
  uint extraout_ECX;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_FUN_000a2218;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_000a2210;
    if (*(int *)(param_1 + 0x112) != 0) {
      (*(code *)**(undefined4 **)(*(int *)(param_1 + 0x112) + 8))();
    }
    param_1 = FUN_00053a30();
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

