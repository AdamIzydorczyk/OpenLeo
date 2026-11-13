
undefined4 * __regparm3 FUN_0002c910(undefined4 param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int unaff_EBX;
  undefined4 *puVar3;
  byte bVar4;
  undefined1 auStack_1814 [2036];
  undefined4 auStack_1020 [1021];
  undefined1 local_2c;
  undefined4 *local_18;
  int local_14;
  
  bVar4 = 0;
  local_2c = 0;
  local_18 = param_2;
  FUN_000749a0();
  puVar1 = (undefined4 *)FUN_00053c90();
  puVar3 = puVar1 + (uint)bVar4 * -2 + 1;
  auStack_1020[0x3fa] = *puVar1;
  auStack_1020[(uint)bVar4 * -2 + 0x3fb] = *puVar3;
  auStack_1020[(uint)bVar4 * -2 + (uint)bVar4 * -2 + 0x3fc] = puVar3[(uint)bVar4 * -2 + 1];
  *(undefined1 *)(auStack_1020 + (uint)bVar4 * -2 + (uint)bVar4 * -2 + 0x3fc + (uint)bVar4 * -2 + 1)
       = *(undefined1 *)(puVar3 + (uint)bVar4 * -2 + 1 + (uint)bVar4 * -2 + 1);
  iVar2 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar2 != 0) {
    iVar2 = FUN_00052eb0();
    if (iVar2 != 0) {
      if (*(int *)(local_14 + 0x5c) == 1) {
        (**(code **)(*(int *)(unaff_EBX + 8) + 0xd0))();
      }
      else if (*(int *)(local_14 + 0x571) == 0) {
        FUN_00052b10();
        *(undefined4 *)(local_14 + 0x571) = 1;
      }
    }
  }
  *(undefined1 *)(local_18 + 3) = local_2c;
  puVar1 = auStack_1020 + 0x3fa;
  puVar3 = local_18;
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
  return local_18;
}

