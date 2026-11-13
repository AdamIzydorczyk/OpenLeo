
void __regparm3 FUN_0005b860(undefined6 *param_1)

{
  int iVar1;
  undefined6 uVar2;
  
  iVar1 = FUN_0005b840();
  if (iVar1 == 0) {
    DAT_000cf16d = 0;
    FUN_00075c3d();
    *(uint *)((int)param_1 + 6) = CONCAT22(DAT_000cf174,DAT_000cf178);
    uVar2 = FUN_00075c5a();
    *param_1 = uVar2;
    FUN_00075c8c();
    FUN_0005b820();
    *(undefined4 *)((int)param_1 + 10) = 0;
    *(undefined4 *)((int)param_1 + 0xe) = 0x37;
  }
  return;
}

