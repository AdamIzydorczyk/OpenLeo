
undefined4 * __regparm3 FUN_00053eb0(undefined4 *param_1,undefined4 param_2)

{
  int *extraout_ECX;
  undefined4 unaff_EBX;
  
  *param_1 = 0;
  param_1[1] = 1;
  param_1[2] = param_2;
  param_1[3] = unaff_EBX;
  *(undefined1 *)(param_1 + 7) = 0;
  FUN_000749a0();
  if (*extraout_ECX == 0) {
    *extraout_ECX = (int)param_1;
    return param_1;
  }
  FUN_00053fe0();
  return param_1;
}

