
undefined4 * __regparm3 FUN_00054910(int param_1,undefined4 *param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  uint uStack_24;
  byte local_20;
  byte local_1f;
  byte local_1e;
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar6 = 0;
  local_18 = 0;
  local_14 = param_2;
  FUN_000749a0();
  if ((*(byte *)(param_1 + 0xa5) & 1) != 0) {
    local_20 = local_20 | 0x80;
  }
  if ((*(byte *)(param_1 + 0xa5) & 0x10) != 0) {
    local_1f = local_1f | 1;
  }
  if ((*(byte *)(param_1 + 0xa5) & 0x20) != 0) {
    local_1e = local_1e | 0x20;
  }
  if (*(int *)(param_1 + 0xba) != 0) {
    cVar1 = ((*(byte *)(param_1 + 0xb6) & 0x10) == 0) + '#';
    uVar3 = (uint)(int)cVar1 >> 5;
    (&uStack_24)[uVar3] = (&uStack_24)[uVar3] | 1 << (sbyte)((uint)(int)cVar1 % 0x20);
  }
  if (*(int *)(param_1 + 0xbe) < 0) {
    local_20 = local_20 | 0x20;
  }
  if (0 < *(int *)(param_1 + 0xbe)) {
    local_20 = local_20 | 0x40;
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

