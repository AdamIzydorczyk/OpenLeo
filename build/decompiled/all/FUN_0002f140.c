
undefined4 __regparm3 FUN_0002f140(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *puVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  undefined4 *puVar3;
  undefined4 *puVar4;
  byte bVar5;
  undefined4 uStack_30;
  undefined1 auStack_2c [4];
  byte local_28;
  undefined4 local_1c;
  undefined4 local_18;
  undefined **local_14;
  undefined4 local_10;
  
  bVar5 = 0;
  uStack_30 = 0x2f155;
  local_10 = param_2;
  FUN_000552e0();
  uStack_30 = 0x2f15d;
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  puVar2 = (undefined4 *)auStack_2c;
  if ((iVar1 != 0) && (puVar2 = (undefined4 *)auStack_2c, *(int *)(param_1 + 0xee) == 0)) {
    uStack_30 = 0x2f17a;
    puVar2 = (undefined4 *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x68))();
    local_1c = *puVar2;
    local_18 = puVar2[1];
    local_14 = &PTR_LAB_00098278;
    uStack_30 = 0x2f19d;
    iVar1 = FUN_0002f230();
    if ((iVar1 != 0) &&
       (((*(byte *)(extraout_ECX_01 + 100) & 0x80) != 0 && (*(int *)(extraout_ECX_01 + 0xee) == 1)))
       ) {
      local_28 = local_28 | 2;
    }
    puVar2 = (undefined4 *)auStack_2c;
    if ((*(int *)(extraout_ECX_01 + 0x5c) == 0x19) &&
       (puVar2 = (undefined4 *)auStack_2c,
       9 < *(int *)(*(int *)(*(int *)(extraout_ECX_01 + 0x50) + 100) + 4))) {
      uStack_30 = 0;
      FUN_0004bc40();
      puVar2 = &uStack_30;
      *(undefined4 *)(param_1 + 0xee) = 1;
    }
  }
  puVar4 = (undefined4 *)puVar2[7];
  *(undefined1 *)(puVar4 + 3) = *(undefined1 *)(puVar2 + 3);
  puVar2[-1] = puVar4;
  puVar3 = puVar2;
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
  return puVar2[7];
}

