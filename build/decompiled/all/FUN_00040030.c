
void __regparm3 FUN_00040030(int param_1)

{
  int iVar1;
  int extraout_EDX;
  
  iVar1 = 0;
  while (iVar1 < *(int *)(param_1 + 0x10e)) {
    FUN_0003fd70();
    iVar1 = extraout_EDX;
  }
  *(undefined4 *)(param_1 + 0x11c) = 0;
  (**(code **)(*(int *)(**(int **)(param_1 + 0x106) + 0x2e) + 0x14))();
  if ((*(int *)(param_1 + 0x11c) != 0) && (*(int *)(param_1 + 0x5c) != 2)) {
    (**(code **)(*(int *)(**(int **)(param_1 + 0x106) + 0x2e) + 8))();
  }
  FUN_00054450();
  return;
}

