
int __regparm3 FUN_000532c0(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  byte bVar9;
  
  bVar9 = 0;
  FUN_000528e0();
  iVar1 = FUN_00021e30();
  *(undefined4 *)(iVar1 + 0x14) = 0;
  *(undefined4 *)(iVar1 + 0x1c) = 0;
  *(undefined1 *)(iVar1 + 0x30) = 0;
  FUN_000749a0();
  *(undefined4 *)(extraout_ECX + 0x15) = 0;
  *(undefined4 *)(extraout_ECX + 0x19) = 0;
  *(undefined4 *)(extraout_ECX + 0x1d) = 0;
  *(undefined4 *)(extraout_ECX + 0x21) = 0;
  *(undefined ***)(extraout_ECX + 0x25) = &PTR_FUN_00098274;
  *(undefined4 *)(extraout_ECX + 0x2d) = 0;
  *(undefined ***)(extraout_ECX + 0x31) = &PTR_LAB_00098278;
  *(undefined4 *)(extraout_ECX + 0x29) = 0;
  *(undefined ***)(extraout_ECX + -0x58) = &PTR_FUN_000a2498;
  *(undefined ***)(extraout_ECX + -0x54) = &PTR_LAB_000a2490;
  uVar2 = (**(code **)(*(int *)(*(int *)(param_2 + 0x54) + 8) + 0x38))();
  *(undefined4 *)(extraout_ECX_00 + 0x54) = uVar2;
  *(undefined4 *)(extraout_ECX_00 + 0x58) = *(undefined4 *)(param_2 + 0x58);
  *(undefined4 *)(extraout_ECX_00 + 0x5c) = *(undefined4 *)(param_2 + 0x5c);
  puVar7 = (undefined4 *)(extraout_ECX_00 + 100 + (uint)bVar9 * -8);
  puVar4 = (undefined4 *)(param_2 + 100 + (uint)bVar9 * -8);
  *(undefined4 *)(extraout_ECX_00 + 0x60) = *(undefined4 *)(param_2 + 0x60);
  puVar8 = puVar7 + (uint)bVar9 * -2 + 1;
  puVar5 = puVar4 + (uint)bVar9 * -2 + 1;
  *puVar7 = *puVar4;
  *puVar8 = *puVar5;
  *(undefined1 *)(puVar8 + (uint)bVar9 * -2 + 1) = *(undefined1 *)(puVar5 + (uint)bVar9 * -2 + 1);
  *(undefined4 *)(extraout_ECX_00 + 0x6d) = *(undefined4 *)(param_2 + 0x6d);
  *(undefined4 *)(extraout_ECX_00 + 0x71) = *(undefined4 *)(param_2 + 0x71);
  *(undefined4 *)(extraout_ECX_00 + 0x95) = *(undefined4 *)(param_2 + 0x95);
  *(undefined4 *)(extraout_ECX_00 + 0x99) = *(undefined4 *)(param_2 + 0x99);
  *(undefined4 *)(extraout_ECX_00 + 0x9d) = *(undefined4 *)(param_2 + 0x9d);
  *(undefined4 *)(extraout_ECX_00 + 0xa1) = 0;
  uVar2 = thunk_FUN_0005c320();
  iVar6 = 1;
  *(undefined4 *)(extraout_ECX_01 + 0x50) = uVar2;
  FUN_000749a0();
  iVar1 = extraout_ECX_02;
  if (0 < *(int *)(extraout_ECX_02 + 0x58)) {
    iVar3 = 4;
    do {
      uVar2 = (**(code **)(*(int *)(*(int *)(iVar3 + *(int *)(param_2 + 0x50)) + 0x44) + 0x14))();
      *(undefined4 *)(*(int *)(extraout_ECX_03 + 0x50) + iVar3) = uVar2;
      iVar6 = iVar6 + 1;
      iVar3 = iVar3 + 4;
      iVar1 = extraout_ECX_03;
    } while (iVar6 <= *(int *)(extraout_ECX_03 + 0x58));
  }
  return iVar1;
}

