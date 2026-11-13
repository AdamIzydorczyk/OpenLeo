
void __regparm3 FUN_00049b70(int param_1,undefined1 param_2,undefined4 param_3)

{
  if (*(int **)(param_1 + 1) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 1) + 0x20))(param_2,param_3);
  }
  return;
}

