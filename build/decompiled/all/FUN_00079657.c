
undefined4 * __regparm3 FUN_00079657(int param_1,undefined4 *param_2)

{
  *param_2 = *(undefined4 *)(param_1 + 8);
  *(undefined4 **)(param_1 + 8) = param_2;
  return param_2;
}

