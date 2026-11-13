
undefined4 __regparm3 FUN_00053030(int param_1,int param_2,undefined4 *param_3)

{
  bool bVar1;
  int *piVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  int *extraout_ECX;
  int *piVar5;
  undefined4 *unaff_EBX;
  undefined8 uVar6;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined **ppuStack_20;
  undefined4 *puStack_10;
  
  puStack_10 = param_3;
  if (*(int *)(param_1 + 0x24) == *(int *)(param_2 + 0x24)) {
    (**(code **)(*(int *)(param_2 + 8) + 0x4c))();
    uVar6 = (**(code **)(*(int *)(param_1 + 8) + 0x4c))();
    piVar5 = (int *)((ulonglong)uVar6 >> 0x20);
    piVar2 = (int *)uVar6;
    if ((((piVar5[3] < piVar2[1]) || (piVar2[3] < piVar5[1])) || (piVar5[2] < *piVar2)) ||
       (piVar2[2] < *piVar5)) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (bVar1) {
      uVar3 = (**(code **)(*(int *)(param_2 + 8) + 0x50))();
      puVar4 = (undefined4 *)(**(code **)(*(int *)(param_2 + 8) + 0x68))(uVar3);
      uStack_28 = *puVar4;
      uStack_24 = puVar4[1];
      ppuStack_20 = &PTR_LAB_00098278;
      (**(code **)(*(int *)(param_2 + 8) + 0x48))(&uStack_28);
      (**(code **)(*(int *)(param_1 + 8) + 0x50))();
      (**(code **)(*(int *)(param_1 + 8) + 0x68))();
      (**(code **)(*(int *)(param_1 + 8) + 0x48))();
      uVar3 = FUN_00042de0();
      return uVar3;
    }
  }
  else {
    FUN_00052dc0();
    piVar2 = (int *)FUN_00052dc0();
    if (((extraout_ECX[3] < piVar2[1]) || (piVar2[3] < extraout_ECX[1])) ||
       ((extraout_ECX[2] < *piVar2 || (piVar2[2] < *extraout_ECX)))) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (bVar1) {
      uVar3 = (**(code **)(*(int *)(param_2 + 8) + 0x50))();
      uVar3 = FUN_00052d30(uVar3);
      (**(code **)(*(int *)(param_2 + 8) + 0x48))(uVar3);
      (**(code **)(*(int *)(param_1 + 8) + 0x50))();
      FUN_00052d30();
      (**(code **)(*(int *)(param_1 + 8) + 0x48))();
      uVar3 = FUN_00042de0();
      return uVar3;
    }
  }
  *unaff_EBX = 0;
  *puStack_10 = 0;
  return 0;
}

