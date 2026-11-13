
void __regparm3 FUN_00040900(int param_1,undefined4 param_2)

{
  if (*(int **)(param_1 + 5) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 5) + 8))(param_2);
  }
  return;
}

