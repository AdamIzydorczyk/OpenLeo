
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_0005b8f0(undefined4 *param_1)

{
  int iVar1;
  
  iVar1 = FUN_0005b840();
  if (iVar1 != 0) {
    FUN_00075c8c();
    *(undefined2 *)(param_1 + 1) = 0;
    *param_1 = 0;
    FUN_0005b820();
    DAT_000cf189 = 1;
    iVar1 = *(int *)((int)param_1 + 6) +
            (int)(CONCAT44(*(int *)((int)param_1 + 10) >> 0x1f,*(undefined4 *)((int)param_1 + 10)) /
                 0x37);
    *(int *)((int)param_1 + 6) = iVar1;
    _DAT_000cf190 = (undefined2)((uint)iVar1 >> 0x10);
    _DAT_000cf194 = *(undefined2 *)((int)param_1 + 6);
    FUN_00075c3d();
  }
  return;
}

