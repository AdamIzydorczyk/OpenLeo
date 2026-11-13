
void __regparm3 FUN_00049ff0(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0xc) - *(int *)(param_1 + 4);
  iVar1 = (iVar1 - (param_2 * iVar1) / 100) / 2;
  *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + iVar1;
  *(int *)(param_1 + 0xc) = *(int *)(param_1 + 0xc) - iVar1;
  return;
}

