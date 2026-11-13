
undefined4 __regparm3 FUN_00052eb0(int param_1,int param_2)

{
  bool bVar1;
  int *piVar2;
  int *piVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *puVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  undefined8 uVar7;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined **ppuStack_24;
  
  if (*(int *)(param_1 + 0x24) == *(int *)(param_2 + 0x24)) {
    (**(code **)(*(int *)(param_2 + 8) + 0x4c))();
    uVar7 = (**(code **)(*(int *)(param_1 + 8) + 0x4c))();
    piVar3 = (int *)((ulonglong)uVar7 >> 0x20);
    piVar2 = (int *)uVar7;
    if ((((piVar3[3] < piVar2[1]) || (piVar2[3] < piVar3[1])) || (piVar3[2] < *piVar2)) ||
       (piVar2[2] < *piVar3)) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (bVar1) {
      uVar4 = (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x50))();
      puVar6 = (undefined4 *)(**(code **)(*(int *)(extraout_ECX_02 + 8) + 0x68))(uVar4);
      uStack_2c = *puVar6;
      uStack_28 = puVar6[1];
      ppuStack_24 = &PTR_LAB_00098278;
      (**(code **)(*(int *)(extraout_ECX_03 + 8) + 0x48))(&uStack_2c);
      (**(code **)(*(int *)(param_1 + 8) + 0x50))();
      (**(code **)(*(int *)(param_1 + 8) + 0x68))();
      (**(code **)(*(int *)(param_1 + 8) + 0x48))();
      iVar5 = FUN_00042a70();
      if (iVar5 != 0) {
        return 1;
      }
    }
    return 0;
  }
  piVar2 = (int *)FUN_00052dc0();
  piVar3 = (int *)FUN_00052dc0();
  if (((piVar2[3] < piVar3[1]) || (piVar3[3] < piVar2[1])) ||
     ((piVar2[2] < *piVar3 || (piVar3[2] < *piVar2)))) {
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  if (bVar1) {
    uVar4 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x50))();
    uVar4 = FUN_00052d30(uVar4);
    (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x48))(uVar4);
    (**(code **)(*(int *)(param_1 + 8) + 0x50))();
    FUN_00052d30();
    (**(code **)(*(int *)(param_1 + 8) + 0x48))();
    iVar5 = FUN_00042a70();
    if (iVar5 != 0) {
      return 1;
    }
  }
  return 0;
}

