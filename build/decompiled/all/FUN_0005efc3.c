
undefined4 __regparm3 FUN_0005efc3(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *extraout_ECX;
  undefined4 local_10;
  undefined4 local_c;
  undefined **local_8;
  
  if ((*(int *)(param_1 + 0x2c) != 2) && (iVar1 = (**(code **)(param_2[2] + 8))(), iVar1 != 0)) {
    local_10 = *param_2;
    local_c = param_2[1];
    local_8 = &PTR_LAB_00098278;
    iVar1 = FUN_00042a70(&local_10,1,*extraout_ECX,extraout_ECX[1],&PTR_LAB_00098278);
    if (iVar1 != 0) {
      return 1;
    }
  }
  return 0;
}

