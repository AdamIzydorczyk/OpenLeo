
undefined4 * __regparm3 FUN_000523f0(int *param_1,undefined4 *param_2)

{
  uint extraout_ECX;
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 auStack_24 [3];
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar4 = 0;
  local_14 = param_2;
  (**(code **)(param_1[0x11] + 8))();
  (**(code **)(param_1[0x11] + 0xc))();
  (**(code **)(*(int *)(*param_1 + 8) + 0xac))();
  (**(code **)(*(int *)(*param_1 + 8) + 0xa8))();
  (**(code **)(param_1[0x11] + 0x10))();
  *(undefined1 *)(local_14 + 3) = local_18;
  puVar2 = auStack_24;
  puVar3 = local_14;
  for (uVar1 = extraout_ECX >> 2; uVar1 != 0; uVar1 = uVar1 - 1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + (uint)bVar4 * -2 + 1;
    puVar3 = puVar3 + (uint)bVar4 * -2 + 1;
  }
  for (uVar1 = extraout_ECX & 3; uVar1 != 0; uVar1 = uVar1 - 1) {
    *(undefined1 *)puVar3 = *(undefined1 *)puVar2;
    puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar4 * -2 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar4 * -2 + 1);
  }
  return local_14;
}

