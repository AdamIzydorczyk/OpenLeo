
undefined4 * __regparm3 FUN_00038350(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int extraout_ECX;
  int *extraout_ECX_00;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 uStack_20;
  byte local_1a;
  undefined1 local_14;
  undefined4 *local_10;
  
  bVar4 = 0;
  local_10 = param_2;
  FUN_0003f7b0();
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if (iVar1 != 0) {
    if ((0x44c < *extraout_ECX_00) && (*extraout_ECX_00 < 0x5dc)) {
      local_1a = local_1a | 0x40;
    }
    if (0x898 < *extraout_ECX_00) {
      local_1a = local_1a | 0x80;
    }
  }
  *(undefined1 *)(local_10 + 3) = local_14;
  puVar2 = &uStack_20;
  puVar3 = local_10;
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
  return local_10;
}

