
int __regparm3 FUN_00060820(int *param_1)

{
  int iVar1;
  
  param_1[0x27] = (int)&PTR_FUN_000a2db4;
  if (*param_1 != 0) {
    FUN_00057870();
    FUN_0005c370();
  }
  *param_1 = 0;
  if (param_1[1] != 0) {
    FUN_000218c0();
    FUN_00057a00();
    FUN_0005c370();
  }
  param_1[1] = 0;
  thunk_FUN_0005c370();
  param_1[5] = 0;
  FUN_0004be80();
  FUN_0004bf10();
  FUN_0004bfa0();
  iVar1 = FUN_0004c030();
  return iVar1 + -0x2c;
}

