
void __regparm3 FUN_00037880(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  if (*(int *)(param_1 + 0x112) == *(int *)(DAT_000cef40 + 0x394)) {
    iVar1 = FUN_00037870(param_3);
    if (-1 < iVar1) {
      *(int *)(*(int *)(DAT_000cef40 + 0x394) + 0x13e) = iVar1;
    }
  }
  FUN_0003e060();
  return;
}

