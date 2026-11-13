
void __regparm3 FUN_0007546f(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 8);
  *(char **)(iVar1 + 0xc) = s_violation_of_function_exception_s_00095c8b;
  *(undefined1 *)(*(int *)(iVar1 + 8) + 0x1c) = 2;
  thunk_FUN_0007c200();
  return;
}

