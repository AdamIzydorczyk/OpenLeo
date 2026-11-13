
void __regparm3 FUN_00056590(int *param_1)

{
  int iVar1;
  
  if ((int *)param_1[1] != (int *)0x0) {
    iVar1 = *param_1;
    *(int *)param_1[1] = iVar1;
    if (iVar1 != 0) {
      *(int *)(iVar1 + 4) = param_1[1];
      *param_1 = 0;
    }
    param_1[1] = 0;
  }
  return;
}

