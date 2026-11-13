
undefined4 FUN_00044020(void)

{
  int iVar1;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *piVar2;
  undefined4 extraout_ECX_01;
  undefined4 uVar3;
  undefined **ppuVar4;
  
  uVar3 = 0;
  FUN_00057990(0,&PTR_FUN_00096200);
  ppuVar4 = &PTR_FUN_000961fc;
  iVar1 = FUN_00044120(uVar3,&PTR_FUN_000961fc);
  if (iVar1 == 0) {
    FUN_00057990(uVar3,ppuVar4,0,&PTR_FUN_00096200,0);
    iVar1 = FUN_00057dd0();
    if (iVar1 == -1) {
      FUN_00057ae0();
    }
    iVar1 = FUN_0005c320();
    uVar3 = 0;
    piVar2 = extraout_ECX;
    if (iVar1 != 0) {
      uVar3 = FUN_00043e70();
      piVar2 = extraout_ECX_00;
    }
    if (piVar2[2] == piVar2[3]) {
      FUN_00022780();
    }
    iVar1 = piVar2[2];
    piVar2[2] = iVar1 + 1;
    *(undefined4 *)(iVar1 * 4 + *piVar2) = uVar3;
    FUN_00057a00();
  }
  FUN_00057a00();
  return extraout_ECX_01;
}

