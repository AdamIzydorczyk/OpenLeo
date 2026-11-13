
undefined4 __regparm3 FUN_00076569(int param_1,int *param_2)

{
  *(byte *)(param_2 + 3) = *(byte *)(param_2 + 3) & 0xef;
  if ((param_2[1] < param_1) || (param_1 < param_2[2] - *param_2)) {
    param_2[1] = 0;
    *param_2 = param_2[2];
    return 1;
  }
  *param_2 = *param_2 + param_1;
  param_2[1] = param_2[1] - param_1;
  return 0;
}

