
undefined4 * FUN_00059c30(void)

{
  int iVar1;
  undefined4 *puVar2;
  uint unaff_EBX;
  undefined4 uVar3;
  
  if ((unaff_EBX & 8) == 0) {
    uVar3 = 0;
    FUN_00057990(0,&PTR_FUN_00096200);
    iVar1 = FUN_00059620(uVar3,&PTR_FUN_000961fc);
    FUN_00057a00();
    if (iVar1 != 0) {
      iVar1 = FUN_0005c320();
      if (iVar1 == 0) {
        return (undefined4 *)0x0;
      }
      puVar2 = (undefined4 *)FUN_00058ef0();
      *puVar2 = &PTR_LAB_000a2948;
      return puVar2;
    }
  }
  return (undefined4 *)0x0;
}

