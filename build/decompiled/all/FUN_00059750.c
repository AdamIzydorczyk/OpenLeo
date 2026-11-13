
undefined4 * FUN_00059750(void)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  char *pcVar4;
  uint unaff_EBX;
  int iVar5;
  undefined4 uVar6;
  
  uVar6 = 0;
  FUN_00057990(0,&PTR_FUN_00096200);
  iVar2 = FUN_00059620(uVar6,&PTR_FUN_000961fc);
  FUN_00057a00();
  if (iVar2 == 0) {
    if ((unaff_EBX & 8) != 0) {
      iVar2 = FUN_00010598();
      if ((iVar2 == 0) || (pcVar4 = (char *)FUN_00010598(), *pcVar4 == '\0')) {
        bVar1 = true;
      }
      else {
        bVar1 = false;
      }
      if (bVar1) {
        FUN_00057990();
        FUN_000588d0();
        iVar2 = FUN_00059550();
        FUN_00057a00();
        iVar5 = 0;
        FUN_00057a00();
        FUN_00057990();
        FUN_00058980();
        FUN_00057a00();
        if (iVar2 != 0) {
          iVar5 = FUN_00058ec0();
        }
        if (iVar5 != 0) {
          FUN_00057990();
          FUN_00010598();
          FUN_00031f20();
          FUN_00057a00();
          iVar2 = FUN_0005c320();
          puVar3 = (undefined4 *)0x0;
          if (iVar2 != 0) {
            puVar3 = (undefined4 *)FUN_00058ef0();
            *puVar3 = &PTR_LAB_000a2918;
          }
          FUN_00057a00();
          return puVar3;
        }
        FUN_00057a00();
      }
    }
  }
  else if ((unaff_EBX & 8) == 0) {
    iVar2 = FUN_0005c320();
    puVar3 = (undefined4 *)0x0;
    if (iVar2 != 0) {
      puVar3 = (undefined4 *)FUN_00058ef0();
      *puVar3 = &PTR_LAB_000a2948;
    }
    return puVar3;
  }
  return (undefined4 *)0x0;
}

