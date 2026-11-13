
undefined4 __regparm3 FUN_0003b570(int *param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint uVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  byte bVar8;
  undefined4 uStack_30;
  undefined1 auStack_2c [16];
  int local_1c;
  int local_18;
  undefined **local_14;
  undefined4 local_10;
  
  bVar8 = 0;
  uStack_30 = 0x3b585;
  local_10 = param_2;
  FUN_0003df00();
  uStack_30 = 0x3b58d;
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  puVar5 = (undefined4 *)auStack_2c;
  if (iVar1 != 0) {
    uStack_30 = 0x3b59d;
    piVar2 = (int *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x68))();
    local_1c = *piVar2;
    local_18 = piVar2[1];
    local_14 = &PTR_LAB_00098278;
    uVar3 = *piVar2 - *param_1;
    uVar4 = (int)uVar3 >> 0x1f;
    puVar5 = (undefined4 *)auStack_2c;
    if (((((int)((uVar3 ^ uVar4) - uVar4) < 0x1f) &&
         (uVar3 = piVar2[1] - param_1[1], uVar4 = (int)uVar3 >> 0x1f,
         puVar5 = (undefined4 *)auStack_2c, (int)((uVar3 ^ uVar4) - uVar4) < 0x1f)) &&
        (puVar5 = (undefined4 *)auStack_2c, *(int *)(extraout_ECX_01 + 0x5c) == 0x1f)) &&
       (puVar5 = (undefined4 *)auStack_2c,
       0x14 < *(int *)(*(int *)(*(int *)(extraout_ECX_01 + 0x50) + 0x7c) + 4))) {
      uStack_30 = 0;
      *(byte *)((int)param_1 + 0x66) = *(byte *)((int)param_1 + 0x66) | 0x40;
      FUN_0004bc40();
      puVar5 = &uStack_30;
    }
  }
  puVar7 = (undefined4 *)puVar5[7];
  *(undefined1 *)(puVar7 + 3) = *(undefined1 *)(puVar5 + 3);
  puVar5[-1] = puVar7;
  puVar6 = puVar5;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar7 = *puVar6;
    puVar6 = puVar6 + (uint)bVar8 * -2 + 1;
    puVar7 = puVar7 + (uint)bVar8 * -2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar7 = *(undefined1 *)puVar6;
    puVar6 = (undefined4 *)((int)puVar6 + (uint)bVar8 * -2 + 1);
    puVar7 = (undefined4 *)((int)puVar7 + (uint)bVar8 * -2 + 1);
  }
  return puVar5[7];
}

