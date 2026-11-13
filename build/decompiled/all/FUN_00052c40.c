
void __regparm3 FUN_00052c40(int param_1)

{
  int iVar1;
  
  if (*(int **)(param_1 + 0x38) != (int *)0x0) {
    iVar1 = *(int *)(param_1 + 0x34);
    **(int **)(param_1 + 0x38) = iVar1;
    if (iVar1 != 0) {
      *(undefined4 *)(iVar1 + 0x38) = *(undefined4 *)(param_1 + 0x38);
      *(undefined4 *)(param_1 + 0x34) = 0;
    }
    *(undefined4 *)(param_1 + 0x38) = 0;
  }
  return;
}

