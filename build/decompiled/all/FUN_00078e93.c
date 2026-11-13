
void __regparm3 FUN_00078e93(int param_1)

{
  int iVar1;
  int iVar2;
  
  if (param_1 == DAT_000a9b5c) {
    DAT_000a9b5c = *(int *)(DAT_000a9b5c + 8);
  }
  if (param_1 == DAT_000cf530) {
    DAT_000cf530 = 0;
  }
  if (param_1 == DAT_000a9b58) {
    DAT_000a9b58 = *(int *)(DAT_000a9b58 + 8);
  }
  iVar1 = *(int *)(param_1 + 4);
  iVar2 = *(int *)(param_1 + 8);
  if (iVar1 != 0) {
    *(int *)(iVar1 + 8) = iVar2;
  }
  if (iVar2 != 0) {
    *(int *)(iVar2 + 4) = iVar1;
  }
  return;
}

