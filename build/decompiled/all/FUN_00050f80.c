
undefined4 * __regparm3 FUN_00050f80(int param_1,undefined4 *param_2)

{
  int iVar1;
  uint uVar2;
  int unaff_EBX;
  undefined4 *puVar3;
  undefined4 *puVar4;
  byte bVar5;
  undefined4 auStack_44 [3];
  undefined1 local_38;
  byte local_24;
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar5 = 0;
  local_38 = 0;
  local_14 = param_2;
  FUN_000749a0();
  iVar1 = FUN_00053ae0();
  if (iVar1 != 0) {
    if (*(int *)(param_1 + 0x11a) == 7) {
      (**(code **)(*(int *)(unaff_EBX + 8) + 0xd0))();
    }
    iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x9c))();
    if (iVar1 != 0) {
      iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x40))();
      if (iVar1 != 0) {
        FUN_00055130();
        FUN_00049580();
      }
      uVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x44))();
      if ((uVar2 & 0x2000) != 0) {
        (**(code **)(*(int *)(unaff_EBX + 8) + 0xd0))();
      }
      local_18 = 0;
      FUN_000749a0();
      local_24 = local_24 | 0x80;
      FUN_00049580();
    }
  }
  *(undefined1 *)(local_14 + 3) = local_38;
  puVar3 = auStack_44;
  puVar4 = local_14;
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
  return local_14;
}

