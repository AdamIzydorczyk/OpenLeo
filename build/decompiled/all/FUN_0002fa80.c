
undefined4 * __regparm3 FUN_0002fa80(int *param_1,undefined4 *param_2)

{
  int iVar1;
  int extraout_ECX;
  int *extraout_ECX_00;
  uint uVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  byte bVar5;
  undefined4 auStack_20 [3];
  undefined1 local_14;
  undefined4 *local_10;
  
  bVar5 = 0;
  local_10 = param_2;
  FUN_000552e0();
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if ((((iVar1 != 0) && (param_1[0x17] == 1)) &&
      (uVar2 = param_1[1] - extraout_ECX_00[1] >> 0x1f,
      (int)((param_1[1] - extraout_ECX_00[1] ^ uVar2) - uVar2) < 0x96)) &&
     (uVar2 = *param_1 - *extraout_ECX_00 >> 0x1f,
     (int)((*param_1 - *extraout_ECX_00 ^ uVar2) - uVar2) < 0x14)) {
    *(byte *)((int)param_1 + 0x66) = *(byte *)((int)param_1 + 0x66) | 0x40;
  }
  *(undefined1 *)(local_10 + 3) = local_14;
  puVar3 = auStack_20;
  puVar4 = local_10;
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
  return local_10;
}

