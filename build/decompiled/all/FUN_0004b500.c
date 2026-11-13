
undefined4 * __regparm2 FUN_0004b500(undefined4 param_1,undefined4 *param_2,int param_3)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int iVar3;
  int unaff_EBX;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined4 auStack_50 [3];
  undefined1 local_44;
  undefined1 local_34;
  undefined4 *local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  bVar6 = 0;
  iVar3 = *(int *)(unaff_EBX + 0x24);
  local_44 = 0;
  local_20 = param_2;
  FUN_000749a0();
  local_14 = extraout_ECX;
  local_1c = 0;
  for (local_10 = 0; local_10 < *(int *)(local_18 + local_14 + 0x44); local_10 = local_10 + 1) {
    local_34 = 0;
    FUN_000749a0();
    for (iVar2 = *(int *)(*(int *)(extraout_ECX_00 + 0x58) + local_1c); iVar2 != 0;
        iVar2 = *(int *)(iVar2 + 0x34)) {
      iVar1 = (**(code **)(*(int *)(iVar2 + 8) + 0x60))();
      iVar2 = extraout_ECX_01;
      if ((iVar1 != 0) &&
         (((*(int *)(extraout_ECX_01 + 0x24) == iVar3 || (param_3 != 0)) &&
          (extraout_ECX_01 != unaff_EBX)))) {
        (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x78))();
        FUN_00049580();
        iVar2 = extraout_ECX_02;
      }
    }
    local_1c = local_1c + 4;
  }
  *(undefined1 *)(local_20 + 3) = local_44;
  puVar4 = auStack_50;
  puVar5 = local_20;
  for (iVar3 = 3; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + (uint)bVar6 * -2 + 1;
    puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
  }
  for (iVar3 = 0; iVar3 != 0; iVar3 = iVar3 + -1) {
    *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
    puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar6 * -2 + 1);
    puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
  }
  return local_20;
}

