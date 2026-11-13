
undefined4 * __regparm3 FUN_0002f8c0(int *param_1,undefined4 *param_2)

{
  int *piVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  byte bVar6;
  undefined4 uStack_30;
  undefined1 local_24;
  int local_20;
  int local_1c;
  undefined **local_18;
  undefined4 *local_14;
  
  bVar6 = 0;
  local_14 = param_2;
  FUN_00053c90();
  piVar1 = (int *)(**(code **)(*(int *)(extraout_ECX + 8) + 0x68))();
  local_20 = *piVar1;
  local_1c = piVar1[1];
  local_18 = &PTR_LAB_00098278;
  if (param_1[0x17] == 1) {
    iVar2 = (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x3c))();
    if (iVar2 != 0) {
      iVar2 = FUN_0002f9b0();
      if (((iVar2 != 0) &&
          (uVar3 = *param_1 - local_20 >> 0x1f,
          (int)((*param_1 - local_20 ^ uVar3) - uVar3) < *(int *)((int)param_1 + 0xee))) &&
         (uVar3 = param_1[1] - local_1c >> 0x1f,
         (int)((param_1[1] - local_1c ^ uVar3) - uVar3) < 0x1e)) {
        *(undefined4 *)((int)param_1 + 0xad) = 0xffffffff;
        uStack_30._3_1_ = uStack_30._3_1_ | 8;
      }
    }
  }
  *(undefined1 *)(local_14 + 3) = local_24;
  puVar4 = &uStack_30;
  puVar5 = local_14;
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
  return local_14;
}

