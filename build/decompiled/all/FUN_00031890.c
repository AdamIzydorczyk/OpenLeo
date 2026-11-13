
void __regparm3 FUN_00031890(int *param_1)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  if (*param_1 != 0) {
    iVar3 = 0;
    if (0 < param_1[2]) {
      iVar2 = 0;
      do {
        iVar1 = param_1[4];
        uVar4 = *(undefined4 *)(*param_1 + iVar2);
        uVar5 = 0;
        FUN_00010598(uVar4,0,&PTR_FUN_00096200);
        FUN_00057990();
        (**(code **)(iVar1 + 4))(uVar4,uVar5,&PTR_FUN_000961fc,*(undefined4 *)(extraout_ECX + 0xc));
        iVar3 = iVar3 + 1;
        FUN_00057a00();
        iVar2 = iVar2 + 0x10;
      } while (iVar3 < param_1[2]);
    }
    FUN_00074bc0();
    thunk_FUN_0005c370();
    *param_1 = 0;
    param_1[2] = 0;
    param_1[3] = 0;
  }
  return;
}

