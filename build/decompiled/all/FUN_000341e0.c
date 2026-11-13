
undefined4 * __regparm3 FUN_000341e0(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  uint uVar4;
  int extraout_ECX_02;
  int unaff_EBX;
  undefined4 *puVar5;
  byte bVar6;
  
  bVar6 = 0;
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if ((iVar1 != 0) && (*(int *)(extraout_ECX + 0x5c) == 1)) {
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x68))();
    FUN_00049fb0(*(undefined4 *)(extraout_ECX_00 + 0xa5),*(undefined4 *)(extraout_ECX_00 + 0xa9),
                 *(undefined4 *)(extraout_ECX_00 + 0xad),*(undefined4 *)(extraout_ECX_00 + 0xb1),
                 &PTR_FUN_00098274);
    iVar2 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar2 != 0) {
      iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
      if (iVar1 == 0) {
        *(undefined1 *)(param_2 + 3) = 0;
        FUN_000749a0();
        *(byte *)(extraout_ECX_02 + 2) = *(byte *)(extraout_ECX_02 + 2) | 0x20;
        return param_2;
      }
      FUN_000749a0();
      FUN_000749a0();
      puVar3 = (undefined4 *)FUN_00049580();
      *(undefined1 *)(param_2 + 3) = *(undefined1 *)(puVar3 + 3);
      puVar5 = param_2;
      for (uVar4 = extraout_ECX_01 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
        *puVar5 = *puVar3;
        puVar3 = puVar3 + (uint)bVar6 * -2 + 1;
        puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
      }
      for (uVar4 = extraout_ECX_01 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
        *(undefined1 *)puVar5 = *(undefined1 *)puVar3;
        puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar6 * -2 + 1);
        puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
      }
      return param_2;
    }
  }
  *(undefined1 *)(param_2 + 3) = 0;
  FUN_000749a0();
  return param_2;
}

