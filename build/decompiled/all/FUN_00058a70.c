
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __regparm3 FUN_00058a70(int param_1,int param_2)

{
  int extraout_ECX;
  
  DAT_000cf0ec = DAT_000cf0ec + 1;
  if ((param_2 < *(int *)(param_1 + 4)) ||
     (*(int *)(param_1 + 8) + *(int *)(param_1 + 4) <= param_2)) {
    FUN_00058ac0();
    _DAT_000cf0f0 = _DAT_000cf0f0 + 1;
    param_2 = extraout_ECX;
  }
  return (param_2 - *(int *)(param_1 + 4)) * 0x1a + *(int *)(param_1 + 0x10);
}

