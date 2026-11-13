
undefined4 __regparm3 FUN_0001f3e0(int param_1,undefined4 param_2)

{
  bool bVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  int unaff_EBX;
  undefined4 *puVar5;
  undefined8 uVar6;
  undefined4 local_14;
  undefined4 local_10;
  
  puVar5 = &local_10;
  local_10 = param_2;
  if (*(int *)(param_1 + 0x5c) == 1) {
    local_14 = 0x1f3ff;
    (**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
    local_14 = 0x1f409;
    uVar6 = (**(code **)(*(int *)(param_1 + 8) + 0x4c))();
    piVar4 = (int *)((ulonglong)uVar6 >> 0x20);
    piVar2 = (int *)uVar6;
    if ((((piVar4[3] < piVar2[1]) || (piVar2[3] < piVar4[1])) || (piVar4[2] < *piVar2)) ||
       (piVar2[2] < *piVar4)) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    puVar5 = &local_10;
    if (bVar1) {
      local_14 = 0x1f43c;
      iVar3 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
      puVar5 = &local_10;
      if ((iVar3 != 0) && (puVar5 = &local_10, *(int *)(unaff_EBX + 0x5c) == 9)) {
        *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
        local_14 = *(undefined4 *)(param_1 + 0x122);
        FUN_0004bc40();
        puVar5 = &local_14;
      }
    }
  }
  puVar5[-1] = 0x1f479;
  FUN_0003df00();
  return *puVar5;
}

