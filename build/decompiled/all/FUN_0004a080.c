
void __regparm3 FUN_0004a080(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = ((*(int *)(param_1 + 0xc) - *(int *)(param_1 + 4)) - param_2) / 2;
  *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + iVar1;
  *(int *)(param_1 + 0xc) = *(int *)(param_1 + 0xc) - iVar1;
  return;
}

