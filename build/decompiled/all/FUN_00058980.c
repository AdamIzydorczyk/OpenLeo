
undefined4 * __regparm3 FUN_00058980(undefined4 *param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  FUN_00057990(0,&PTR_FUN_00096200);
  iVar1 = FUN_00057e30(uVar2,&PTR_FUN_000961fc);
  if (iVar1 != -1) {
    FUN_00057e70();
  }
  *param_1 = 0;
  param_1[1] = &PTR_FUN_00096200;
  FUN_00057990();
  param_1[1] = &PTR_FUN_000961fc;
  FUN_00057a00();
  return param_1;
}

