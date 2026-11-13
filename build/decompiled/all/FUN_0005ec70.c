
int __regparm3 FUN_0005ec70(int param_1,uint param_2,undefined4 param_3)

{
  uint extraout_ECX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *(undefined ***)(param_1 + 0x10) = &PTR_FUN_000a2e68;
  if (*(int *)(param_1 + 0xc) != 0) {
    FUN_000218c0(param_3);
    FUN_00057a00();
    FUN_0005c370();
    param_2 = extraout_ECX;
  }
  *(undefined4 *)(param_1 + 0xc) = 0;
  if ((param_2 & 2) != 0) {
    FUN_0005c370();
  }
  return param_1;
}

