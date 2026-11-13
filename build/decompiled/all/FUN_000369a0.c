
undefined4 * __regparm3 FUN_000369a0(int *param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int iVar4;
  int extraout_ECX_04;
  int extraout_ECX_05;
  uint uVar5;
  uint uVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  byte bVar9;
  undefined4 uStack_20;
  undefined1 local_14;
  undefined4 *local_10;
  
  bVar9 = 0;
  local_14 = 0;
  local_10 = param_2;
  FUN_000749a0();
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if (iVar1 == 0) {
    iVar1 = 1;
  }
  else {
    iVar1 = 6;
  }
  iVar2 = (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x3c))();
  if (iVar2 == 0) {
    iVar2 = 1;
  }
  else {
    iVar2 = 7;
  }
  piVar3 = (int *)(**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x6c))();
  uVar5 = *param_1 - (*piVar3 + -2);
  uVar6 = (int)uVar5 >> 0x1f;
  iVar4 = extraout_ECX_02;
  if ((int)((uVar5 ^ uVar6) - uVar6) <= iVar1) {
    iVar1 = FUN_00052eb0();
    iVar4 = extraout_ECX_03;
    if (iVar1 != 0) {
      uStack_20._1_1_ = uStack_20._1_1_ | 4;
    }
  }
  piVar3 = (int *)(**(code **)(*(int *)(iVar4 + 8) + 0x6c))();
  uVar5 = *param_1 - (*piVar3 + -2);
  uVar6 = (int)uVar5 >> 0x1f;
  if ((int)((uVar5 ^ uVar6) - uVar6) <= iVar2) {
    iVar1 = (**(code **)(*(int *)(extraout_ECX_04 + 8) + 0x6c))();
    if (param_1[1] < *(int *)(iVar1 + 4) + 0x19) {
      iVar1 = (**(code **)(*(int *)(extraout_ECX_05 + 8) + 0x6c))();
      if (*(int *)(iVar1 + 4) < param_1[1]) {
        uStack_20._1_1_ = uStack_20._1_1_ | 8;
      }
    }
  }
  *(undefined1 *)(local_10 + 3) = local_14;
  puVar7 = &uStack_20;
  puVar8 = local_10;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar8 = *puVar7;
    puVar7 = puVar7 + (uint)bVar9 * -2 + 1;
    puVar8 = puVar8 + (uint)bVar9 * -2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar8 = *(undefined1 *)puVar7;
    puVar7 = (undefined4 *)((int)puVar7 + (uint)bVar9 * -2 + 1);
    puVar8 = (undefined4 *)((int)puVar8 + (uint)bVar9 * -2 + 1);
  }
  return local_10;
}

