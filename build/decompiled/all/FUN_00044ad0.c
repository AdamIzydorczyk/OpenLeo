
void __regparm3 FUN_00044ad0(int *param_1)

{
  int *extraout_ECX;
  int *extraout_ECX_00;
  char local_90;
  byte local_8f;
  char local_8e;
  char local_8d;
  short local_8c;
  short local_8a;
  short local_88;
  short local_86;
  char local_4f;
  
  if (*(int **)((int)param_1 + 0xd) != (int *)0x0) {
    (**(code **)(**(int **)((int)param_1 + 0xd) + 0x1c))();
    param_1 = extraout_ECX;
  }
  if ((((local_90 != '\n') || (local_8f < 5)) || (local_8e != '\x01')) ||
     ((local_8d != '\b' || (local_4f != '\x01')))) {
    ll_printf(&DAT_000a92d4,s_Bledny_format_pliku___s_000917f8,*(undefined4 *)((int)param_1 + 0x11))
    ;
    ll_rethrow_report();
    param_1 = extraout_ECX_00;
  }
  *param_1 = ((int)local_88 - (int)local_8c) + 1;
  param_1[1] = ((int)local_86 - (int)local_8a) + 1;
  return;
}

