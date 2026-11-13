
void __regparm3 FUN_0003ee80(int *param_1)

{
  int iVar1;
  int extraout_ECX;
  int *extraout_ECX_00;
  undefined4 *puVar2;
  int *piVar3;
  undefined4 local_88;
  int local_84;
  undefined **local_80;
  undefined4 local_7c;
  int local_78;
  undefined **local_74;
  undefined4 local_70;
  int local_6c;
  undefined **local_68;
  int local_64;
  int local_60;
  undefined **local_5c;
  undefined4 local_58;
  int local_54;
  undefined **local_50;
  int local_4c [2];
  undefined **local_44;
  int local_40;
  int local_3c;
  undefined **local_38;
  undefined1 *local_34;
  int *local_30;
  undefined4 *local_2c;
  undefined4 *local_28;
  int *local_24;
  undefined4 *local_20;
  int *local_1c;
  
  local_34 = (undefined1 *)&local_88;
  local_2c = &local_7c;
  local_7c = *(undefined4 *)param_1[0x12];
  local_78 = ((undefined4 *)param_1[0x12])[1];
  local_74 = &PTR_LAB_00098278;
  puVar2 = (undefined4 *)(param_1[0x13] * 0xd + param_1[0x12]);
  local_88 = *puVar2;
  local_84 = puVar2[1];
  local_80 = &PTR_LAB_00098278;
  if (local_78 < local_84) {
    local_30 = &local_64;
    local_64 = *(int *)param_1[0x12];
    local_60 = ((int *)param_1[0x12])[1];
    local_5c = &PTR_LAB_00098278;
    *param_1 = local_64 + 1;
    local_28 = &local_58;
    local_58 = *(undefined4 *)param_1[0x12];
    iVar1 = ((undefined4 *)param_1[0x12])[1];
    local_50 = &PTR_LAB_00098278;
    local_54 = iVar1;
  }
  else {
    local_24 = &local_40;
    piVar3 = (int *)(param_1[0x13] * 0xd + param_1[0x12]);
    local_40 = *piVar3;
    local_3c = piVar3[1];
    local_38 = &PTR_LAB_00098278;
    *param_1 = local_40 + -1;
    local_20 = &local_70;
    puVar2 = (undefined4 *)(param_1[0x12] + param_1[0x13] * 0xd);
    local_70 = *puVar2;
    iVar1 = puVar2[1];
    local_68 = &PTR_LAB_00098278;
    local_6c = iVar1;
  }
  param_1[1] = iVar1;
  param_1[1] = param_1[1] + -1;
  (**(code **)(*(int *)(param_1[0xf] + 8) + 0x6c))();
  (**(code **)(*(int *)(*(int *)(extraout_ECX + 0x3c) + 8) + 0x68))();
  local_1c = local_4c;
  local_44 = &PTR_LAB_00098278;
  (*(code *)PTR_FUN_0009827c)();
  iVar1 = local_1c[1];
  *extraout_ECX_00 = *extraout_ECX_00 + *local_1c;
  extraout_ECX_00[1] = extraout_ECX_00[1] + iVar1;
  return;
}

