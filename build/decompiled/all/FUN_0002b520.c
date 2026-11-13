
void __regparm3 FUN_0002b520(int *param_1)

{
  int iVar1;
  
  param_1[2] = param_1[2] + 1;
  if (param_1[2] == *(int *)(param_1[3] + param_1[1] * 8 + 4)) {
    iVar1 = param_1[1];
    param_1[2] = 0;
    param_1[1] = iVar1 + 1;
    if (iVar1 + 1 == *param_1) {
      param_1[1] = 0;
    }
  }
  return;
}

