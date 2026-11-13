
undefined4 * __regparm3 FUN_0005ec20(undefined4 *param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  *param_1 = *param_2;
  uVar2 = param_2[1];
  param_1[2] = &PTR_LAB_00098278;
  param_1[3] = 0;
  param_1[1] = uVar2;
  param_1[4] = &PTR_FUN_000a2e68;
  iVar1 = FUN_0005c320();
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_00043e70();
  }
  param_1[3] = uVar2;
  return param_1;
}

