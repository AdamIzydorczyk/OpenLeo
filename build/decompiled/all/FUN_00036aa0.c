
undefined4 * __regparm3 FUN_00036aa0(int *param_1,undefined4 *param_2)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  uint uVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  byte bVar7;
  undefined4 uStack_20;
  undefined1 local_14;
  undefined4 *local_10;
  
  bVar7 = 0;
  local_14 = 0;
  local_10 = param_2;
  FUN_000749a0();
  piVar1 = (int *)(**(code **)(*(int *)(extraout_ECX + 8) + 0x6c))();
  if (*param_1 + -8 < *piVar1) {
    piVar1 = (int *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x6c))();
    if (*piVar1 < *param_1 + 8) {
      iVar2 = (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x6c))();
      uVar3 = (*(int *)(iVar2 + 4) + -0x1f) - param_1[1];
      uVar4 = (int)uVar3 >> 0x1f;
      if ((int)((uVar3 ^ uVar4) - uVar4) < 10) {
        iVar2 = (**(code **)(*(int *)(extraout_ECX_02 + 8) + 0x70))();
        if (iVar2 == 1) {
          uStack_20._2_1_ = uStack_20._2_1_ | 2;
        }
      }
    }
  }
  *(undefined1 *)(local_10 + 3) = local_14;
  puVar5 = &uStack_20;
  puVar6 = local_10;
  for (iVar2 = 3; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + (uint)bVar7 * -2 + 1;
    puVar6 = puVar6 + (uint)bVar7 * -2 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)puVar6 = *(undefined1 *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar7 * -2 + 1);
    puVar6 = (undefined4 *)((int)puVar6 + (uint)bVar7 * -2 + 1);
  }
  return local_10;
}

