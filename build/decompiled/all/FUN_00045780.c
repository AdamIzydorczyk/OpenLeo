
undefined4 * __regparm3 FUN_00045780(undefined4 *param_1,int *param_2,undefined4 param_3)

{
  undefined4 uVar1;
  
  *param_1 = 0;
  param_1[1] = &PTR_FUN_000a1000;
  if (*param_2 != 0) {
    uVar1 = (**(code **)(*(int *)(*param_2 + 4) + 0x14))(param_3);
    *param_1 = uVar1;
  }
  return param_1;
}

