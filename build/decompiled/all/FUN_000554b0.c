
void __regparm3 FUN_000554b0(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_00055470();
  iVar2 = iVar1 + 1;
  if (iVar2 < *(int *)(param_1 + 0x3c)) {
    do {
      *(undefined4 *)(*(int *)(param_1 + 0x40) + iVar1 * 4) =
           *(undefined4 *)(*(int *)(param_1 + 0x40) + 4 + iVar1 * 4);
      iVar2 = iVar2 + 1;
    } while (iVar2 < *(int *)(param_1 + 0x3c));
  }
  *(undefined4 *)(*(int *)(param_1 + 0x40) + -4 + *(int *)(param_1 + 0x3c) * 4) = 0;
  *(int *)(param_1 + 0x3c) = *(int *)(param_1 + 0x3c) + -1;
  return;
}

