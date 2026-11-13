
undefined4 __regparm3 FUN_0005a170(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  int extraout_ECX;
  undefined4 *puVar3;
  byte bVar4;
  char *pcVar5;
  undefined4 uVar6;
  undefined **ppuVar7;
  
  bVar4 = 0;
  if (param_2 == 0) {
    return 0;
  }
  uVar6 = 0;
  FUN_00057990();
  ppuVar7 = &PTR_FUN_000961fc;
  iVar2 = FUN_00059ff0();
  puVar3 = (undefined4 *)(*(int *)(extraout_ECX + 0x14) + iVar2 * 0x10);
  if (puVar3[3] != 0) {
    pcVar5 = (char *)0x0;
    FUN_00057990(0,&PTR_FUN_00096200,uVar6,ppuVar7);
    FUN_00057ae0(pcVar5,&PTR_FUN_000961fc);
    if (pcVar5 != (char *)0x0) {
      iVar2 = -1;
      do {
        if (iVar2 == 0) break;
        iVar2 = iVar2 + -1;
        cVar1 = *pcVar5;
        pcVar5 = pcVar5 + (uint)bVar4 * -2 + 1;
      } while (cVar1 != '\0');
    }
    (**(code **)(*(int *)puVar3[3] + 0x20))();
    FUN_00057a00();
  }
  uVar6 = (**(code **)(*(int *)*puVar3 + 0xc))();
  FUN_00057a00();
  return uVar6;
}

