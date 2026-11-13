
undefined4 * __regparm3 FUN_000504e0(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  uint extraout_ECX;
  uint uVar3;
  undefined4 *puVar4;
  byte bVar5;
  undefined4 uStack_4c;
  byte local_47;
  byte local_46;
  undefined1 local_40;
  undefined4 local_2c [3];
  undefined1 local_20;
  undefined4 *local_1c;
  undefined4 *local_18;
  
  bVar5 = 0;
  local_1c = param_2;
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xf8))();
  if (((iVar1 != 0) && (*(int *)(param_1 + 0xd2) != 0)) && ((*(byte *)(param_1 + 0xa5) & 2) != 0)) {
    FUN_000551f0();
  }
  if (((*(byte *)(param_1 + 0xa5) & 0x40) != 0) && (DAT_000cef5c != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 0;
  }
  if (((*(byte *)(param_1 + 0xa5) & 0x80) != 0) && (DAT_000cef60 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 1;
  }
  if (((*(byte *)(param_1 + 0xa6) & 1) != 0) && (DAT_000cef64 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 2;
  }
  if (((*(byte *)(param_1 + 0xa6) & 2) != 0) && (DAT_000cef68 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 3;
  }
  if (((*(byte *)(param_1 + 0xa6) & 0x40) != 0) && (DAT_000cef6c != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 4;
  }
  if (((*(byte *)(param_1 + 0xa6) & 0x80) != 0) && (DAT_000cef70 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 5;
  }
  if (((*(byte *)(param_1 + 0xa7) & 1) != 0) && (DAT_000cef74 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 6;
  }
  if (((*(byte *)(param_1 + 0xa7) & 2) != 0) && (DAT_000cef78 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 7;
  }
  if (((*(byte *)(param_1 + 0xa7) & 4) != 0) && (DAT_000cef7c != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 8;
  }
  if (((*(byte *)(param_1 + 0xa7) & 8) != 0) && (DAT_000cef80 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 9;
  }
  local_40 = 0;
  FUN_000749a0();
  switch(*(undefined4 *)(param_1 + 0xee)) {
  case 0:
    local_47 = local_47 | 2;
    break;
  case 1:
    local_47 = local_47 | 4;
    break;
  case 2:
    local_47 = local_47 | 8;
    break;
  case 3:
    local_47 = local_47 | 0x10;
    break;
  case 4:
    local_47 = local_47 | 0x20;
    break;
  case 5:
    local_47 = local_47 | 0x40;
    break;
  case 6:
    local_47 = local_47 | 0x80;
    break;
  case 7:
    local_46 = local_46 | 1;
    break;
  case 8:
    local_46 = local_46 | 2;
    break;
  case 9:
    local_46 = local_46 | 4;
    break;
  case 10:
    local_46 = local_46 | 8;
    break;
  case 0xb:
    local_46 = local_46 | 0x10;
  }
  local_20 = local_40;
  puVar2 = &uStack_4c;
  puVar4 = local_2c;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar4 = *puVar2;
    puVar2 = puVar2 + (uint)bVar5 * -2 + 1;
    puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar4 = *(undefined1 *)puVar2;
    puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar5 * -2 + 1);
    puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar5 * -2 + 1);
  }
  FUN_00054910();
  local_18 = local_1c;
  puVar2 = (undefined4 *)FUN_00049580();
  *(undefined1 *)(local_18 + 3) = *(undefined1 *)(puVar2 + 3);
  for (uVar3 = extraout_ECX >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *local_18 = *puVar2;
    puVar2 = puVar2 + (uint)bVar5 * -2 + 1;
    local_18 = local_18 + (uint)bVar5 * -2 + 1;
  }
  for (uVar3 = extraout_ECX & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined1 *)local_18 = *(undefined1 *)puVar2;
    puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar5 * -2 + 1);
    local_18 = (undefined4 *)((int)local_18 + (uint)bVar5 * -2 + 1);
  }
  return local_1c;
}

