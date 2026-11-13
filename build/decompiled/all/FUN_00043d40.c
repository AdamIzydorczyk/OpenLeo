
undefined4 __regparm3 FUN_00043d40(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  byte bVar4;
  undefined4 uVar5;
  undefined **ppuVar6;
  
  bVar4 = 0;
  uVar5 = 0;
  FUN_00057990(0,&PTR_FUN_00096200,param_1,param_2);
  ppuVar6 = &PTR_FUN_000961fc;
  FUN_00057e30(uVar5,&PTR_FUN_000961fc);
  pcVar2 = (char *)FUN_00010598();
  if (pcVar2 != (char *)0x0) {
    iVar3 = -1;
    do {
      if (iVar3 == 0) break;
      iVar3 = iVar3 + -1;
      cVar1 = *pcVar2;
      pcVar2 = pcVar2 + (uint)bVar4 * -2 + 1;
    } while (cVar1 != '\0');
  }
  do {
    FUN_00057e70();
    iVar3 = FUN_00057e30(uVar5,ppuVar6);
  } while (iVar3 != -1);
  FUN_00057a70();
  FUN_00057a00();
  return param_1;
}

