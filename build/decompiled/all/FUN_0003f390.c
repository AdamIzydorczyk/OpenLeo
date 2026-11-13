
undefined4 * __regparm3 FUN_0003f390(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  uint extraout_ECX_03;
  uint uVar4;
  int extraout_ECX_04;
  int unaff_EBX;
  undefined4 *puVar5;
  byte bVar6;
  
  bVar6 = 0;
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar1 != 0) {
    if (*(int *)(extraout_ECX + 0x5c) == 1) {
      (**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
      iVar1 = FUN_00044ec0();
      if (iVar1 != 0) {
        *(byte *)(extraout_ECX_00 + 0x66) = *(byte *)(extraout_ECX_00 + 0x66) | 0x40;
      }
    }
    else if (*(int *)(extraout_ECX + 0x5c) == 2) {
      iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x68))();
      iVar2 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
      if (iVar2 != 0) {
        *(undefined1 *)(param_2 + 3) = 0;
        FUN_000749a0();
        *(byte *)(extraout_ECX_02 + 1) = *(byte *)(extraout_ECX_02 + 1) | 1;
        return param_2;
      }
      FUN_00049fb0(*(undefined4 *)(extraout_ECX_01 + 0xb1),*(undefined4 *)(extraout_ECX_01 + 0xb5),
                   *(undefined4 *)(extraout_ECX_01 + 0xb9),*(undefined4 *)(extraout_ECX_01 + 0xbd),
                   &PTR_FUN_00098274);
      iVar2 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
      if (iVar2 != 0) {
        iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
        if (iVar1 == 0) {
          *(undefined1 *)(param_2 + 3) = 0;
          FUN_000749a0();
          *(byte *)(extraout_ECX_04 + 2) = *(byte *)(extraout_ECX_04 + 2) | 0x20;
          return param_2;
        }
        FUN_000749a0();
        FUN_000749a0();
        puVar3 = (undefined4 *)FUN_00049580();
        *(undefined1 *)(param_2 + 3) = *(undefined1 *)(puVar3 + 3);
        puVar5 = param_2;
        for (uVar4 = extraout_ECX_03 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
          *puVar5 = *puVar3;
          puVar3 = puVar3 + (uint)bVar6 * -2 + 1;
          puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
        }
        for (uVar4 = extraout_ECX_03 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
          *(undefined1 *)puVar5 = *(undefined1 *)puVar3;
          puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar6 * -2 + 1);
          puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
        }
        return param_2;
      }
    }
  }
  *(undefined1 *)(param_2 + 3) = 0;
  FUN_000749a0();
  return param_2;
}

