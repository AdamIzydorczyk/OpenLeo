
undefined4 * __regparm3 FUN_0001c900(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 *extraout_ECX;
  undefined4 unaff_EBX;
  
  param_1[2] = 0;
  param_1[1] = param_2;
  param_1[3] = unaff_EBX;
  param_1[4] = &PTR_FUN_000963c0;
  if (param_1[3] == 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = thunk_FUN_0005c320(param_3);
    param_1 = extraout_ECX;
  }
  *param_1 = uVar1;
  return param_1;
}

