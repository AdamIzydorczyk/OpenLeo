
int * __regparm3 FUN_00059260(int *param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint extraout_ECX;
  
  if ((param_2 & 4) != 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    return param_1;
  }
  *param_1 = (int)&PTR_LAB_000a2894;
  (**(code **)(*param_1 + 0x28))(param_3);
  if (param_1[1] != 0) {
    FUN_00058bb0();
    FUN_0005c370();
  }
  if (param_1[2] != 0) {
    FUN_0007630a();
  }
  iVar1 = FUN_00057a00();
  if ((extraout_ECX & 2) != 0) {
    FUN_0005c370();
  }
  return (int *)(iVar1 + -0xc);
}

