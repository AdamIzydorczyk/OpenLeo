
void __regparm3 FUN_00044c00(int param_1)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  
  if (*(int **)(param_1 + 0xd) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0xd) + 0xc))();
    param_1 = extraout_ECX;
  }
  if (*(int **)(param_1 + 0xd) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0xd) + 0x10))();
    param_1 = extraout_ECX_00;
  }
  if (*(int **)(param_1 + 0xd) == (int *)0x0) {
    iVar1 = -1;
  }
  else {
    iVar1 = (**(code **)(**(int **)(param_1 + 0xd) + 0x24))();
    param_1 = extraout_ECX_01;
  }
  if (iVar1 != 0xc) {
    ll_printf(&DAT_000a92d4,s_Bledny_format_pliku___s_00091810,*(undefined4 *)(param_1 + 0x11));
    ll_rethrow_report();
    param_1 = extraout_ECX_02;
  }
  if (*(int **)(param_1 + 0xd) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0xd) + 0x1c))();
    param_1 = extraout_ECX_03;
  }
  if (*(int **)(param_1 + 0xd) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0xd) + 8))();
  }
  return;
}

