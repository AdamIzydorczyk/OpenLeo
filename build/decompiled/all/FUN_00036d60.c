
undefined4 __regparm3 FUN_00036d60(int *param_1,int param_2)

{
  uint uVar1;
  int unaff_EBX;
  
  if (((param_2 < *param_1) || (param_1[1] < param_2)) ||
     (uVar1 = unaff_EBX - *(int *)(&DAT_000a906c + param_1[2] * 4) >> 0x1f,
     9 < (int)((unaff_EBX - *(int *)(&DAT_000a906c + param_1[2] * 4) ^ uVar1) - uVar1))) {
    return 0;
  }
  return 1;
}

