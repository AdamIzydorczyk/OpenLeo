
void __regparm3 FUN_0002e4c0(int param_1)

{
  int iVar1;
  
  if (*(int *)(param_1 + 0x14e) == 0) {
    iVar1 = *(int *)(param_1 + 0xad) + 0x22;
    *(int *)(param_1 + 0xad) = iVar1;
    if (100 < iVar1) {
      *(undefined4 *)(param_1 + 0xad) = 100;
    }
    *(undefined4 *)(param_1 + 0x14e) = 1;
  }
  return;
}

