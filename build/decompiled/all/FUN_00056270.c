
void __regparm3 FUN_00056270(int *param_1)

{
  FUN_00056310();
  if ((*param_1 != 0) && (*(int *)((int)param_1 + 0x5e) != 0)) {
    DAT_000afb6c = 1;
    FUN_00012a70();
    FUN_0005ba40();
    FUN_0001cbf0();
    param_1[7] = 0;
    param_1[3] = 1;
  }
  return;
}

