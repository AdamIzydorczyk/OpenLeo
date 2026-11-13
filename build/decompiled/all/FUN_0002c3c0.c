
undefined4 * __regparm3 FUN_0002c3c0(int *param_1,undefined4 *param_2)

{
  int *piVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined8 uVar7;
  undefined4 uStack_24;
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar6 = 0;
  local_18 = 0;
  local_14 = param_2;
  FUN_000749a0();
  piVar1 = (int *)(**(code **)(*(int *)(extraout_ECX + 8) + 0x6c))();
  uVar3 = *param_1 - *piVar1 >> 0x1f;
  if ((int)((*param_1 - *piVar1 ^ uVar3) - uVar3) < 0x6a) {
    piVar1 = (int *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x6c))();
    uVar3 = *param_1 - *piVar1 >> 0x1f;
    if (0x4f < (int)((*param_1 - *piVar1 ^ uVar3) - uVar3)) {
      (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x6c))();
      uVar7 = (**(code **)(*(int *)(extraout_ECX_02 + 8) + 0x70))();
      if (0 < (int)((ulonglong)uVar7 >> 0x20) / (int)uVar7) {
        iVar2 = (**(code **)(*(int *)(extraout_ECX_03 + 8) + 0x6c))();
        if (*(int *)(iVar2 + 4) - param_1[1] < 0x56) {
          iVar2 = (**(code **)(*(int *)(extraout_ECX_04 + 8) + 0x6c))();
          if (0x40 < *(int *)(iVar2 + 4) - param_1[1]) {
            uStack_24._1_1_ = uStack_24._1_1_ | 4;
          }
        }
      }
    }
  }
  *(undefined1 *)(local_14 + 3) = local_18;
  puVar4 = &uStack_24;
  puVar5 = local_14;
  for (iVar2 = 3; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
    puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
    puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
    puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
  }
  return local_14;
}

