
undefined4 * __regparm3 FUN_0002c4a0(int *param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  undefined4 *puVar5;
  byte bVar6;
  undefined1 auStack_1810 [2036];
  undefined4 auStack_101c [1019];
  byte local_2e;
  undefined1 local_28;
  undefined4 *local_14;
  
  bVar6 = 0;
  local_28 = 0;
  local_14 = param_2;
  FUN_000749a0();
  puVar1 = (undefined4 *)FUN_000552e0();
  puVar5 = puVar1 + (uint)bVar6 * -2 + 1;
  auStack_101c[0x3fa] = *puVar1;
  auStack_101c[(uint)bVar6 * -2 + 0x3fb] = *puVar5;
  auStack_101c[(uint)bVar6 * -2 + (uint)bVar6 * -2 + 0x3fc] = puVar5[(uint)bVar6 * -2 + 1];
  *(undefined1 *)(auStack_101c + (uint)bVar6 * -2 + (uint)bVar6 * -2 + 0x3fc + (uint)bVar6 * -2 + 1)
       = *(undefined1 *)(puVar5 + (uint)bVar6 * -2 + 1 + (uint)bVar6 * -2 + 1);
  iVar2 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if (iVar2 == 0) goto LAB_0002c555;
  iVar2 = FUN_00052eb0();
  if (iVar2 == 0) goto LAB_0002c555;
  piVar3 = (int *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x6c))();
  iVar2 = extraout_ECX_01;
  if (*param_1 - *piVar3 < 1) {
LAB_0002c514:
    piVar3 = (int *)(**(code **)(*(int *)(iVar2 + 8) + 0x6c))();
    if (-1 < *param_1 - *piVar3) goto LAB_0002c555;
    iVar2 = (**(code **)(*(int *)(extraout_ECX_03 + 8) + 0x70))();
    if (-1 < iVar2) goto LAB_0002c555;
  }
  else {
    iVar4 = (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x70))();
    iVar2 = extraout_ECX_02;
    if (iVar4 < 1) goto LAB_0002c514;
  }
  iVar2 = *(int *)(*(int *)(param_1[0x17] * 4 + param_1[0x14]) + 4);
  if ((iVar2 != 0xd) && (iVar2 != 0x1b)) {
    local_2e = local_2e | 0x80;
  }
LAB_0002c555:
  *(undefined1 *)(local_14 + 3) = local_28;
  puVar1 = auStack_101c + 0x3fa;
  puVar5 = local_14;
  for (iVar2 = 3; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar5 = *puVar1;
    puVar1 = puVar1 + (uint)bVar6 * -2 + 1;
    puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)puVar5 = *(undefined1 *)puVar1;
    puVar1 = (undefined4 *)((int)puVar1 + (uint)bVar6 * -2 + 1);
    puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
  }
  return local_14;
}

