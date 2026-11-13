
undefined4 * __regparm3 FUN_00030670(int *param_1,undefined4 *param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int *extraout_ECX_01;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined4 auStack_20 [3];
  undefined1 local_14;
  undefined4 *local_10;
  
  bVar6 = 0;
  local_10 = param_2;
  FUN_000552e0();
  iVar1 = FUN_00052eb0();
  iVar2 = extraout_ECX;
  if (iVar1 != 0) {
    iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0xc))();
    iVar2 = extraout_ECX_00;
    if (iVar1 == DAT_000cecb0) {
      *(undefined4 *)((int)param_1 + 0xad) = 0xffffffff;
      goto LAB_000306e6;
    }
  }
  iVar2 = (**(code **)(*(int *)(iVar2 + 8) + 0x3c))();
  if (((iVar2 != 0) &&
      (uVar3 = *param_1 - *extraout_ECX_01 >> 0x1f,
      (int)((*param_1 - *extraout_ECX_01 ^ uVar3) - uVar3) < 0x65)) &&
     (uVar3 = param_1[1] - extraout_ECX_01[1] >> 0x1f,
     (int)((param_1[1] - extraout_ECX_01[1] ^ uVar3) - uVar3) < 0xc9)) {
    *(byte *)((int)param_1 + 0x66) = *(byte *)((int)param_1 + 0x66) | 0x40;
  }
LAB_000306e6:
  *(undefined1 *)(local_10 + 3) = local_14;
  puVar4 = auStack_20;
  puVar5 = local_10;
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
  return local_10;
}

