
int __regparm3 FUN_00045380(int param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  
  if (((*(int *)(param_1 + 8) == 0) || (*param_2 < **(int **)(param_1 + 4))) ||
     (*(int *)(*(int *)(param_1 + 8) * 0xd + (int)*(int **)(param_1 + 4)) < *param_2)) {
    return 0;
  }
  iVar2 = 1;
  for (iVar1 = 0xd; *(int *)(*(int *)(param_1 + 4) + iVar1) < *param_2; iVar1 = iVar1 + 0xd) {
    iVar2 = iVar2 + 1;
  }
  return iVar2;
}

