
int __regparm3 FUN_00060960(int param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_ECX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370(param_3);
    return param_1;
  }
  *(undefined ***)(param_1 + 0x9c) = &PTR_FUN_000a2d90;
  thunk_FUN_0005c370(param_3);
  *(undefined4 *)(param_1 + 0xa0) = 0;
  iVar1 = FUN_00060820();
  if ((extraout_ECX & 2) != 0) {
    FUN_0005c370();
  }
  return iVar1;
}

