
undefined4 * __regparm3 FUN_0002c9f0(int param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int extraout_ECX;
  undefined4 *puVar3;
  byte bVar4;
  undefined1 auStack_1810 [2036];
  undefined4 auStack_101c [1021];
  undefined1 local_28;
  undefined4 *local_14;
  
  bVar4 = 0;
  local_28 = 0;
  local_14 = param_2;
  FUN_000749a0();
  puVar1 = (undefined4 *)FUN_000552e0();
  puVar3 = puVar1 + (uint)bVar4 * -2 + 1;
  auStack_101c[0x3fa] = *puVar1;
  auStack_101c[(uint)bVar4 * -2 + 0x3fb] = *puVar3;
  auStack_101c[(uint)bVar4 * -2 + (uint)bVar4 * -2 + 0x3fc] = puVar3[(uint)bVar4 * -2 + 1];
  *(undefined1 *)(auStack_101c + (uint)bVar4 * -2 + (uint)bVar4 * -2 + 0x3fc + (uint)bVar4 * -2 + 1)
       = *(undefined1 *)(puVar3 + (uint)bVar4 * -2 + 1 + (uint)bVar4 * -2 + 1);
  iVar2 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if (iVar2 != 0) {
    iVar2 = FUN_00052eb0();
    if (iVar2 != 0) {
      *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
    }
  }
  *(undefined1 *)(local_14 + 3) = local_28;
  puVar1 = auStack_101c + 0x3fa;
  puVar3 = local_14;
  for (iVar2 = 3; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *puVar1;
    puVar1 = puVar1 + (uint)bVar4 * -2 + 1;
    puVar3 = puVar3 + (uint)bVar4 * -2 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)puVar3 = *(undefined1 *)puVar1;
    puVar1 = (undefined4 *)((int)puVar1 + (uint)bVar4 * -2 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar4 * -2 + 1);
  }
  return local_14;
}

