
void __regparm3 FUN_00041d40(undefined4 param_1,int param_2)

{
  int extraout_ECX;
  int extraout_ECX_00;
  
  if (*(int **)(param_2 + 1) != (int *)0x0) {
    (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
    param_2 = extraout_ECX;
  }
  if (*(int **)(param_2 + 1) != (int *)0x0) {
    (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
    param_2 = extraout_ECX_00;
  }
  if (*(int **)(param_2 + 1) != (int *)0x0) {
    (**(code **)(**(int **)(param_2 + 1) + 0x1c))();
  }
  return;
}

