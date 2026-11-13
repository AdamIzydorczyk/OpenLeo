
int __regparm3 FUN_0003f5a0(int param_1,uint param_2)

{
  uint extraout_ECX;
  
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_FUN_000a0780;
    *(undefined ***)(param_1 + 0x44) = &PTR_LAB_000a0774;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_000a076c;
    if ((DAT_000cf0e4 != *(int *)(param_1 + 0x48)) && (*(int *)(param_1 + 0x48) != 0)) {
      (**(code **)(*(int *)(*(int *)(param_1 + 0x48) + 8) + 0x14))();
    }
    FUN_00044e40();
    param_1 = FUN_00052960();
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

