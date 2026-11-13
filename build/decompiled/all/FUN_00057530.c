
int __regparm3 FUN_00057530(int param_1,int param_2)

{
  return (*(int *)(param_1 + 0x14) * *(int *)(param_2 * 4 + *(int *)(param_1 + 0x24))) /
         *(int *)(*(int *)(param_1 + 0x1c) * 4 + *(int *)(param_1 + 0x24)) +
         *(int *)(param_2 * 4 + *(int *)(param_1 + 0x28));
}

