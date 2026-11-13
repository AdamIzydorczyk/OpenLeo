
int __regparm3 FUN_00060a80(int param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_ECX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *(undefined ***)(param_1 + 0x9c) = &PTR_LAB_000a2d48;
  if (*(int *)(param_1 + 0xa4) != 0) {
    FUN_000218c0(param_3);
    FUN_00057a00();
    FUN_0005c370();
  }
  *(undefined4 *)(param_1 + 0xa4) = 0;
  iVar1 = FUN_00060960();
  if ((extraout_ECX & 2) != 0) {
    FUN_0005c370();
  }
  return iVar1;
}

