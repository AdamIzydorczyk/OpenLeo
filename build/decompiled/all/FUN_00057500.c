
int __regparm3 FUN_00057500(int param_1,int param_2)

{
  return (*(int *)(param_1 + 0x10) * *(int *)(*(int *)(param_1 + 0x20) + param_2 * 4)) /
         *(int *)(*(int *)(param_1 + 0x20) + *(int *)(param_1 + 0x1c) * 4);
}

