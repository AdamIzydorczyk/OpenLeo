
undefined4 * __regparm3 FUN_0002ddf0(int param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  int unaff_EBX;
  
  param_1 = unaff_EBX * 0x1c + param_1;
  *param_2 = *(undefined4 *)(param_1 + 0x10);
  uVar1 = *(undefined4 *)(param_1 + 0x14);
  param_2[2] = &PTR_LAB_00098278;
  param_2[1] = uVar1;
  return param_2;
}

