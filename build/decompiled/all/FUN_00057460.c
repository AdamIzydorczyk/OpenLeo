
void __regparm3 FUN_00057460(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < *(int *)(param_1 + 0x18)) {
    iVar1 = 0;
    do {
      *(int *)(iVar1 + *(int *)(param_1 + 0x28)) =
           (*(short *)(*(int *)(iVar1 + *(int *)(param_1 + 0x44)) + 2) + -200) -
           ((*(short *)(*(int *)(*(int *)(param_1 + 0x44) + *(int *)(param_1 + 0x1c) * 4) + 2) +
            -200) * *(int *)(iVar1 + *(int *)(param_1 + 0x24))) / 100;
      iVar2 = iVar2 + 1;
      iVar1 = iVar1 + 4;
    } while (iVar2 < *(int *)(param_1 + 0x18));
  }
  return;
}

