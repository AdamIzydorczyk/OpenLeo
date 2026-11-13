
void __regparm3 FUN_00056b20(undefined2 *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 extraout_ECX;
  uint extraout_ECX_00;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  int local_50;
  undefined2 local_4c;
  
  bVar6 = 0;
  FUN_00057990();
  FUN_00057ae0();
  FUN_00057ae0();
  iVar1 = FUN_000104b8();
  *(undefined4 *)(iVar1 + 9) = extraout_ECX;
  iVar1 = FUN_000104b8();
  *(undefined ***)(iVar1 + 8) = &PTR_FUN_0009d574;
  FUN_00044a50();
  param_1[1] = local_4c;
  *param_1 = (short)(local_50 / 0x50);
  puVar2 = (undefined4 *)thunk_FUN_0005c320();
  *(undefined4 **)(param_1 + 2) = puVar2;
  for (iVar1 = 0; iVar1 < (short)param_1[1]; iVar1 = iVar1 + 1) {
    FUN_00044b80();
    puVar4 = puVar2;
    puVar5 = (undefined4 *)(local_50 + (int)puVar2);
    for (uVar3 = extraout_ECX_00 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
      puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
    }
    for (uVar3 = extraout_ECX_00 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
      puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
      puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
    }
    puVar2 = (undefined4 *)((int)puVar2 + local_50 + 0x50);
  }
  FUN_00044ab0();
  FUN_00049ba0();
  FUN_00057a00();
  FUN_00049af0();
  FUN_00057a00();
  FUN_00057a00();
  return;
}

