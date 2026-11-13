
undefined4 * __regparm3 FUN_00057a70(undefined4 param_1,undefined4 *param_2)

{
  undefined4 extraout_ECX;
  undefined4 uVar1;
  
  uVar1 = 0;
  FUN_00057990(0,&PTR_FUN_00096200);
  FUN_00057ae0(uVar1,&PTR_FUN_000961fc);
  *param_2 = 0;
  param_2[1] = extraout_ECX;
  FUN_00057990();
  param_2[1] = &PTR_FUN_000961fc;
  FUN_00057a00();
  return param_2;
}

