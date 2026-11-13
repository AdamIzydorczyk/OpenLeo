
undefined4 * __regparm3 FUN_0003f7b0(int *param_1,undefined4 *param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 local_40;
  undefined1 local_34;
  int local_30 [2];
  undefined **local_28;
  int local_24;
  int local_20;
  undefined **local_1c;
  undefined4 *local_18;
  int *local_14;
  
  bVar4 = 0;
  local_34 = 0;
  local_18 = param_2;
  FUN_000749a0();
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if (iVar1 != 0) {
    local_24 = *param_1;
    local_20 = param_1[1];
    local_1c = &PTR_LAB_00098278;
    local_14 = local_30;
    local_28 = &PTR_LAB_00098278;
    (*(code *)PTR_FUN_0009827c)();
    local_24 = local_24 + *local_14;
    local_20 = local_20 + local_14[1];
    iVar1 = (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x6c))
                      (local_24 + -5,local_20 + -0x1e,local_24 + 5,local_20 + 0x1e,&PTR_FUN_00098274
                      );
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar1 != 0) {
      local_40._3_1_ = local_40._3_1_ | 0x80;
    }
  }
  *(undefined1 *)(local_18 + 3) = local_34;
  puVar2 = &local_40;
  puVar3 = local_18;
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
  return local_18;
}

