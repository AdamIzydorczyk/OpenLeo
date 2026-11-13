
void __regparm3 FUN_00056340(int param_1)

{
  int iVar1;
  
  if (*(int *)(param_1 + 0xc) != 0) {
    iVar1 = *(int *)(param_1 + 0x1c);
    while (0 < iVar1) {
      iVar1 = *(int *)(param_1 + 0x14);
    }
  }
  *(undefined4 *)(param_1 + 0x1c) = 0;
  FUN_00056310();
  return;
}

