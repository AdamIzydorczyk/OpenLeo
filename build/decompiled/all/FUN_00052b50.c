
void __regparm3 FUN_00052b50(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x10);
  while (iVar1 != 0) {
    iVar1 = **(int **)(param_1 + 0x10);
    (**(code **)(*(int *)(param_1 + 8) + 0x10))();
    FUN_0005c370();
    *(int *)(param_1 + 0x10) = iVar1;
  }
  return;
}

