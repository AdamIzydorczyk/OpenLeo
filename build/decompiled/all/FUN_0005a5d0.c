
void __regparm3 FUN_0005a5d0(int *param_1)

{
  char cVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int extraout_ECX;
  int extraout_ECX_00;
  int *unaff_EBX;
  undefined4 *puVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  char *pcVar10;
  byte bVar11;
  int *piVar12;
  undefined4 uVar13;
  char *pcVar14;
  undefined **ppuVar15;
  
  bVar11 = 0;
  pcVar14 = (char *)0x0;
  FUN_00057990();
  ppuVar15 = &PTR_FUN_000961fc;
  FUN_00059f80();
  if (pcVar14 == (char *)0x0) {
    iVar4 = 0;
  }
  else {
    uVar3 = 0xffffffff;
    pcVar10 = pcVar14;
    do {
      if (uVar3 == 0) break;
      uVar3 = uVar3 - 1;
      cVar1 = *pcVar10;
      pcVar10 = pcVar10 + (uint)bVar11 * -2 + 1;
    } while (cVar1 != '\0');
    iVar4 = ~uVar3 - 1;
  }
  uVar2 = (**(code **)(*param_1 + 0x10))();
  uVar13 = 0;
  piVar12 = unaff_EBX;
  FUN_00057990(unaff_EBX,0,&PTR_FUN_00096200);
  if (param_1[7] == param_1[8]) {
    FUN_000317d0(piVar12,uVar13,&PTR_FUN_000961fc,uVar2,pcVar14,ppuVar15);
  }
  iVar9 = param_1[7];
  param_1[7] = iVar9 + 1;
  puVar6 = (undefined4 *)(param_1[5] + iVar9 * 0x10);
  *puVar6 = piVar12;
  FUN_0005a850();
  FUN_0005a840();
  puVar6[3] = uVar2;
  piVar7 = param_1 + 5;
  if (-1 < param_1[7] + -2) {
    iVar8 = (param_1[7] + -2) * 0x10;
    iVar9 = iVar8 + 0x10;
    do {
      pcVar14 = *(char **)(*piVar7 + iVar8 + 4);
      if (pcVar14 == (char *)0x0) {
        iVar5 = 0;
      }
      else {
        uVar3 = 0xffffffff;
        do {
          if (uVar3 == 0) break;
          uVar3 = uVar3 - 1;
          cVar1 = *pcVar14;
          pcVar14 = pcVar14 + (uint)bVar11 * -2 + 1;
        } while (cVar1 != '\0');
        iVar5 = ~uVar3 - 1;
      }
      if (iVar5 < iVar4) {
        puVar6 = (undefined4 *)(*piVar7 + iVar8);
        *(undefined4 *)(*piVar7 + iVar9) = *puVar6;
        FUN_00010598();
        FUN_00031f20();
        *(undefined4 *)(extraout_ECX + 0xc) = puVar6[3];
        *(int **)(*piVar7 + iVar8) = piVar12;
        FUN_00010598();
        FUN_00031f20();
        *(undefined4 *)(extraout_ECX_00 + 0xc) = uVar2;
      }
      iVar9 = iVar9 + -0x10;
      iVar8 = iVar8 + -0x10;
    } while (-1 < iVar8);
  }
  (**(code **)(*unaff_EBX + 0x38))();
  FUN_00057a00();
  FUN_00057a00();
  return;
}

