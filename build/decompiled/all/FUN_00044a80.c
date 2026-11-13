
void __regparm3 FUN_00044a80(int param_1)

{
  int iVar1;
  
  iVar1 = 0;
  if (0 < *(int *)(param_1 + 4)) {
    do {
      iVar1 = iVar1 + 1;
      (**(code **)(*(int *)(param_1 + 0x26) + 0xc))();
    } while (iVar1 < *(int *)(param_1 + 4));
  }
  return;
}

