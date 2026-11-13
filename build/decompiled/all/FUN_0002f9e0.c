
undefined4 * __regparm3 FUN_0002f9e0(int *param_1,undefined4 *param_2)

{
  int iVar1;
  int *piVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint uVar3;
  uint uVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  byte bVar7;
  undefined4 auStack_24 [3];
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar7 = 0;
  local_14 = param_2;
  FUN_00053c90();
  if (param_1[0x17] == 1) {
    iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
    if (iVar1 != 0) {
      piVar2 = (int *)(**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x68))();
      uVar3 = *param_1 - *piVar2 >> 0x1f;
      if ((int)((*param_1 - *piVar2 ^ uVar3) - uVar3) < *(int *)((int)param_1 + 0xee)) {
        iVar1 = (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x68))();
        uVar3 = param_1[1] - *(int *)(iVar1 + 4);
        uVar4 = (int)uVar3 >> 0x1f;
        if ((int)((uVar3 ^ uVar4) - uVar4) < 0x1e) {
          *(undefined4 *)((int)param_1 + 0xad) = 0xffffffff;
        }
      }
    }
  }
  *(undefined1 *)(local_14 + 3) = local_18;
  puVar5 = auStack_24;
  puVar6 = local_14;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + (uint)bVar7 * -2 + 1;
    puVar6 = puVar6 + (uint)bVar7 * -2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar6 = *(undefined1 *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar7 * -2 + 1);
    puVar6 = (undefined4 *)((int)puVar6 + (uint)bVar7 * -2 + 1);
  }
  return local_14;
}

