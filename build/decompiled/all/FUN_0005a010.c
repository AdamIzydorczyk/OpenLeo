
int __regparm3 FUN_0005a010(int param_1,undefined4 *param_2)

{
  char cVar1;
  int iVar2;
  int extraout_ECX;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  byte bVar7;
  char *pcVar8;
  
  bVar7 = 0;
  iVar4 = *(int *)(param_1 + 0x1c) + -1;
  iVar6 = 0;
  if (0 < iVar4) {
    iVar5 = 0;
    do {
      pcVar8 = (char *)0x0;
      FUN_00057990(0,&PTR_FUN_00096200);
      iVar2 = FUN_00057c60(pcVar8,&PTR_FUN_000961fc);
      if (iVar2 == 0) {
        if (pcVar8 == (char *)0x0) goto LAB_0005a0a0;
        iVar4 = -1;
        goto code_r0x0005a095;
      }
      FUN_00057a00();
      iVar5 = iVar5 + 0x10;
      iVar6 = iVar6 + 1;
    } while (iVar5 < extraout_ECX);
  }
  return iVar4;
  while( true ) {
    iVar4 = iVar4 + -1;
    cVar1 = *pcVar8;
    pcVar8 = pcVar8 + (uint)bVar7 * -2 + 1;
    if (cVar1 == '\0') break;
code_r0x0005a095:
    if (iVar4 == 0) break;
  }
LAB_0005a0a0:
  FUN_00057e70();
  if ((char *)*param_2 == (char *)0x0) {
    iVar4 = 0;
  }
  else {
    uVar3 = 0xffffffff;
    pcVar8 = (char *)*param_2;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar8;
      pcVar8 = pcVar8 + (uint)bVar7 * -2 + 1;
    } while (cVar1 != '\0');
    iVar4 = ~uVar3 - 1;
  }
  if ((iVar4 != 0) && (*(char *)*param_2 == '\\')) {
    FUN_00057e70();
  }
  FUN_00057a00();
  return iVar6;
}

