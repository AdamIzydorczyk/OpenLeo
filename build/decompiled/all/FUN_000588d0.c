
undefined4 * __regparm3 FUN_000588d0(undefined4 *param_1)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  byte bVar4;
  undefined4 uVar5;
  
  bVar4 = 0;
  uVar5 = 0;
  FUN_00057990(0,&PTR_FUN_00096200);
  iVar2 = FUN_00057e30(uVar5,&PTR_FUN_000961fc);
  if (iVar2 == -1) {
    FUN_00057990();
  }
  else {
    pcVar3 = (char *)FUN_00010598();
    if (pcVar3 != (char *)0x0) {
      iVar2 = -1;
      do {
        if (iVar2 == 0) break;
        iVar2 = iVar2 + -1;
        cVar1 = *pcVar3;
        pcVar3 = pcVar3 + (uint)bVar4 * -2 + 1;
      } while (cVar1 != '\0');
    }
    FUN_00057e70();
  }
  *param_1 = 0;
  param_1[1] = &PTR_FUN_00096200;
  FUN_00057990();
  param_1[1] = &PTR_FUN_000961fc;
  FUN_00057a00();
  return param_1;
}

