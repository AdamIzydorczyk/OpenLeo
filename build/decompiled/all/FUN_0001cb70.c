
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_0001cb70(int param_1,int param_2,undefined4 param_3)

{
  int unaff_EBX;
  
  _DAT_000ceb4c = 4;
  _DAT_000ceb54 = (short)param_2 * 2;
  _DAT_000ceb58 = (short)unaff_EBX * 2;
  *(int *)(param_1 + 4) = param_2 * 2;
  *(int *)(param_1 + 8) = unaff_EBX * 2;
  FUN_00075c3d(param_3);
  return;
}

