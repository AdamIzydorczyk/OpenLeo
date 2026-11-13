
undefined4 * __regparm3 FUN_00036b50(int *param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  bool bVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  int iVar12;
  undefined4 *puVar13;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  undefined4 *puVar14;
  byte bVar15;
  
  bVar15 = 0;
  FUN_000749a0();
  iVar12 = param_1[1];
  iVar11 = *param_1;
  iVar1 = param_1[1];
  iVar2 = *param_1;
  iVar3 = param_1[1];
  iVar4 = *param_1;
  iVar5 = param_1[1];
  iVar6 = *param_1;
  iVar9 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if ((iVar9 == 0) || (param_1[0x17] != 1)) goto LAB_00036d07;
  piVar10 = (int *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x6c))();
  iVar9 = *piVar10;
  iVar7 = piVar10[1];
  if ((iVar9 < iVar2 + -0x32) ||
     (((iVar11 + -0xf < iVar9 || (iVar7 < iVar1 + -10)) || (iVar12 + 10 < iVar7)))) {
    bVar8 = false;
  }
  else {
    bVar8 = true;
  }
  iVar12 = extraout_ECX_01;
  if (bVar8) {
    iVar11 = (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x70))();
    iVar12 = extraout_ECX_02;
    if (iVar11 != 1) goto LAB_00036c45;
  }
  else {
LAB_00036c45:
    if (((iVar9 < iVar6 + 0x46) || (iVar4 + 0x69 < iVar9)) ||
       ((iVar7 < iVar5 + -10 || (iVar3 + 10 < iVar7)))) {
      bVar8 = false;
    }
    else {
      bVar8 = true;
    }
    if (!bVar8) goto LAB_00036d07;
    iVar12 = (**(code **)(*(int *)(iVar12 + 8) + 0x70))();
    if (iVar12 != -1) goto LAB_00036d07;
  }
  FUN_000749a0();
  FUN_00049580();
  if ((*(int *)(extraout_ECX_03 + 0x5c) == 0xe) &&
     (6 < *(int *)(*(int *)(*(int *)(extraout_ECX_03 + 0x50) + 0x38) + 4))) {
    FUN_000749a0();
    FUN_00049580();
    FUN_000371b0();
  }
LAB_00036d07:
  FUN_0003df00();
  puVar13 = (undefined4 *)FUN_00049580();
  *(undefined1 *)(param_2 + 3) = *(undefined1 *)(puVar13 + 3);
  puVar14 = param_2;
  for (iVar12 = 3; iVar12 != 0; iVar12 = iVar12 + -1) {
    *puVar14 = *puVar13;
    puVar13 = puVar13 + (uint)bVar15 * -2 + 1;
    puVar14 = puVar14 + (uint)bVar15 * -2 + 1;
  }
  for (iVar12 = 0; iVar12 != 0; iVar12 = iVar12 + -1) {
    *(undefined1 *)puVar14 = *(undefined1 *)puVar13;
    puVar13 = (undefined4 *)((int)puVar13 + (uint)bVar15 * -2 + 1);
    puVar14 = (undefined4 *)((int)puVar14 + (uint)bVar15 * -2 + 1);
  }
  return param_2;
}

