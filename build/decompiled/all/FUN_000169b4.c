
undefined4 __regparm3 FUN_000169b4(int param_1)

{
  int extraout_ECX;
  int unaff_EBX;
  int iVar1;
  
  if (unaff_EBX == 0) {
    if (*(int *)(param_1 + 1) == 0) {
      return 0;
    }
    iVar1 = **(int **)(param_1 + 1);
  }
  else {
    if (unaff_EBX != 1) {
      if (*(int **)(param_1 + 1) == (int *)0x0) {
        return 0;
      }
      (**(code **)(**(int **)(param_1 + 1) + 0x10))();
      return 0;
    }
    if (*(int **)(param_1 + 1) != (int *)0x0) {
      (**(code **)(**(int **)(param_1 + 1) + 0xc))();
      param_1 = extraout_ECX;
    }
    if (*(int **)(param_1 + 1) == (int *)0x0) {
      return 0;
    }
    iVar1 = **(int **)(param_1 + 1);
  }
  (**(code **)(iVar1 + 8))();
  return 0;
}

