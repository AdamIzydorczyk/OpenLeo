
void __regparm3 FUN_00052c10(int param_1)

{
  int *extraout_EDX;
  
  FUN_00052c40();
  *(int *)(param_1 + 0x34) = *extraout_EDX;
  *extraout_EDX = param_1;
  *(int **)(param_1 + 0x38) = extraout_EDX;
  if (*(int *)(param_1 + 0x34) != 0) {
    *(int *)(*(int *)(param_1 + 0x34) + 0x38) = param_1 + 0x34;
  }
  return;
}

