
undefined4 * __regparm3 FUN_00027280(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 uStack_24;
  byte local_1f;
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar4 = 0;
  local_14 = param_2;
  FUN_000504e0();
  if (2 < *(int *)(param_1 + 0xee)) {
    *(int *)(param_1 + 0xee) = *(short *)(param_1 + 0x12a) + 2;
  }
  local_1f = local_1f & 0xef;
  if (*(int *)(param_1 + 0xee) == 6) {
    local_1f = local_1f | 0x10;
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

