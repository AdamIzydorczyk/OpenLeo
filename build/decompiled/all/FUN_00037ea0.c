
undefined4 * __regparm3 FUN_00037ea0(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int *piVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  undefined4 *puVar3;
  undefined4 *puVar4;
  byte bVar5;
  longdouble lVar6;
  undefined4 uStack_28;
  byte local_24;
  undefined1 local_1c;
  undefined4 *local_18;
  int local_14;
  int local_10;
  
  bVar5 = 0;
  local_1c = 0;
  local_18 = param_2;
  FUN_000749a0();
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x6c))();
  iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
  if (iVar1 != 0) {
    local_24 = local_24 | 1;
  }
  iVar1 = (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x6c))();
  local_14 = *(int *)(iVar1 + 4) + -500;
  lVar6 = (longdouble)FUN_00075d96();
  local_10 = (int)ROUND(lVar6);
  piVar2 = (int *)(**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x6c))();
  if (local_10 < *piVar2) {
    piVar2 = (int *)(**(code **)(*(int *)(extraout_ECX_02 + 8) + 0x6c))();
    if (*piVar2 < 0x55c) {
      iVar1 = (**(code **)(*(int *)(extraout_ECX_03 + 8) + 0x6c))();
      if (0x1a4 < *(int *)(iVar1 + 4)) {
        uStack_28._3_1_ = uStack_28._3_1_ | 0x10;
      }
    }
  }
  *(undefined1 *)(local_18 + 3) = local_1c;
  puVar3 = &uStack_28;
  puVar4 = local_18;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + (uint)bVar5 * -2 + 1;
    puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar4 = *(undefined1 *)puVar3;
    puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar5 * -2 + 1);
    puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar5 * -2 + 1);
  }
  return local_18;
}

