
void __regparm3 FUN_000317d0(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  
  iVar1 = param_1[3];
  param_1[3] = iVar1 + param_2;
  if (iVar1 + param_2 == 0) {
    FUN_00074bc0();
    thunk_FUN_0005c370();
    *param_1 = 0;
    return;
  }
  thunk_FUN_0005c320();
  iVar1 = FUN_00076bb7();
  iVar2 = 0;
  if (0 < param_1[2]) {
    do {
      puVar3 = (undefined4 *)(*param_1 + iVar2 * 0x10);
      puVar4 = (undefined4 *)(iVar2 * 0x10 + iVar1);
      *puVar4 = *puVar3;
      FUN_00010598();
      FUN_00031f20();
      puVar4[3] = puVar3[3];
      iVar2 = iVar2 + 1;
    } while (iVar2 < param_1[2]);
  }
  FUN_00074bc0();
  thunk_FUN_0005c370();
  *param_1 = iVar1;
  return;
}

