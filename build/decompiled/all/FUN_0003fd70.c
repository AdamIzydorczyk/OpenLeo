
void __regparm3 FUN_0003fd70(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  iVar2 = 0;
  if (*(int *)(param_1 + 8) < 1) {
    return;
  }
  do {
    iVar1 = *(int *)(param_1 + 0x2a);
    *(undefined4 *)(iVar1 + 4 + iVar2) = 0;
    iVar2 = iVar2 + 8;
    iVar3 = iVar3 + 1;
    *(undefined4 *)(*(int *)(param_1 + 0x2a) + -8 + iVar2) = *(undefined4 *)(iVar1 + -4 + iVar2);
  } while (iVar3 < *(int *)(param_1 + 8));
  return;
}

