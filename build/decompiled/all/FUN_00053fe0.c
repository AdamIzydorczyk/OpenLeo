
void __regparm3 FUN_00053fe0(int *param_1,int param_2)

{
  int iVar1;
  
  iVar1 = *param_1;
  while (iVar1 != 0) {
    param_1 = (int *)*param_1;
    iVar1 = *param_1;
  }
  *param_1 = param_2;
  return;
}

