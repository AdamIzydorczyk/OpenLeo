
void __regparm3 FUN_0004a050(int *param_1,int param_2)

{
  int iVar1;
  
  iVar1 = ((param_1[2] - *param_1) - param_2) / 2;
  *param_1 = *param_1 + iVar1;
  param_1[2] = param_1[2] - iVar1;
  return;
}

