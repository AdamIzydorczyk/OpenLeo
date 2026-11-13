
void __regparm3 FUN_00055d20(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  if (*param_1 != 0) {
    FUN_00056170(param_2,param_3);
    FUN_00012a64();
    iVar1 = *(int *)((int)param_1 + 0x5a);
    *param_1 = 0;
    while (iVar1 != 0) {
      FUN_00056590();
      iVar1 = *(int *)((int)param_1 + 0x5a);
    }
    FUN_00057990();
    FUN_000561a0();
  }
  return;
}

