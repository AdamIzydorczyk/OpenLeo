
undefined4 * __regparm3 FUN_0003c270(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 uStack_24;
  undefined1 local_18;
  undefined4 *local_14;
  undefined1 *local_10;
  
  bVar4 = 0;
  local_10 = (undefined1 *)&uStack_24;
  local_18 = 0;
  local_14 = param_2;
  FUN_000749a0();
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if (iVar1 != 0) {
    iVar1 = (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x68))();
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar1 != 0) {
      uStack_24._1_1_ = uStack_24._1_1_ | 4;
    }
    iVar1 = (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x68))();
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar1 != 0) {
      uStack_24._1_1_ = uStack_24._1_1_ | 1;
    }
  }
  *(undefined1 *)(local_14 + 3) = local_18;
  puVar2 = &uStack_24;
  puVar3 = local_14;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + (uint)bVar4 * -2 + 1;
    puVar3 = puVar3 + (uint)bVar4 * -2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar3 = *(undefined1 *)puVar2;
    puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar4 * -2 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar4 * -2 + 1);
  }
  return local_14;
}

