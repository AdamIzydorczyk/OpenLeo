
undefined8 __regparm3 FUN_000603b0(int param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_EDX;
  int iVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int local_2c;
  int local_28;
  int local_1c;
  
  local_28 = 0;
  iVar8 = 0;
  iVar9 = 0;
  (**(code **)(*(int *)(param_1 + 0x9c) + 0xc))();
  FUN_0005f310();
  *(undefined4 *)(param_1 + 0x28) = 0;
  iVar5 = 0;
  if (*(int *)(param_1 + 0x34) != 0) {
    FUN_0001cb70();
  }
  iVar1 = param_1 + 8;
  piVar6 = (int *)(param_1 + 0x2c);
  do {
    if (local_28 != 0) {
      (**(code **)(*(int *)(param_1 + 0x9c) + 0x10))();
      FUN_0005f310();
      return CONCAT44(param_2,local_2c);
    }
    local_2c = local_28;
    *(int *)(param_1 + 8) = DAT_000ceb88 >> 1;
    *(int *)(param_1 + 0xc) = DAT_000ceb8c >> 1;
    uVar2 = DAT_000ceb94 & 1;
    DAT_000ceb94 = DAT_000ceb94 & 0xfffffffe;
    if ((uVar2 == 0) || (iVar9 != 0)) {
      iVar7 = 0;
    }
    else {
      iVar7 = 1;
      iVar9 = 4;
    }
    (**(code **)(*(int *)(param_1 + 0x9c) + 0x14))(iVar7,iVar8,iVar9,iVar1);
    iVar8 = 0;
    for (local_1c = 0; local_1c < *(int *)(param_1 + 0x34); local_1c = local_1c + 1) {
      if (*(int *)(*(int *)(iVar8 + *piVar6) + 0x2c) != 2) {
        *(undefined4 *)(*(int *)(iVar8 + *piVar6) + 0x2c) = 1;
      }
      if (*(int *)(param_1 + 0x1c) != 0) {
        iVar3 = (**(code **)(*(int *)(*(int *)(iVar8 + *piVar6) + 0x10) + 0x10))();
        iVar8 = extraout_ECX;
        if (iVar3 != 0) {
          iVar5 = local_1c;
        }
      }
      iVar8 = iVar8 + 4;
    }
    if (*(int *)(param_1 + 0x34) != 0) {
      *(undefined4 *)(*(int *)(*piVar6 + iVar5 * 4) + 0x2c) = 0;
    }
    (**(code **)(*(int *)(param_1 + 0x9c) + 8))();
    (*(code *)**(undefined4 **)(param_1 + 0x9c))();
    (**(code **)(*(int *)(param_1 + 0x9c) + 4))();
    iVar8 = 0;
    for (iVar3 = 0; iVar3 < *(int *)(param_1 + 0x34); iVar3 = iVar3 + 1) {
      (**(code **)(*(int *)(*(int *)(iVar8 + *piVar6) + 0x10) + 4))();
      iVar8 = extraout_EDX;
    }
    if (*(int *)(param_1 + 0x34) != 0) {
      iVar8 = *(int *)(*piVar6 + iVar5 * 4);
      local_2c = *(int *)(iVar8 + 0x28);
      (**(code **)(*(int *)(iVar8 + 0x10) + 0xc))();
    }
    if (*(int *)(param_1 + 0x1c) != 0) {
      (**(code **)(*(int *)(**(int **)(*(int *)(param_1 + 4) + 8) + 0x33) + 0x14))();
    }
    (**(code **)(*(int *)(param_1 + 0x9c) + 0x18))();
    iVar8 = FUN_0001c550();
    if ((*(int *)(param_1 + 0x1c) != 0) && (*(int *)(param_1 + 0x34) != 0)) {
      if ((iVar8 == 0) &&
         ((iVar7 == 0 ||
          (iVar8 = (**(code **)(*(int *)(*(int *)(*piVar6 + iVar5 * 4) + 0x10) + 0x10))(),
          iVar8 == 0)))) {
        iVar8 = 0;
      }
      else {
        iVar8 = 1;
      }
    }
    if (*(int *)(param_1 + 0x34) != 0) {
      iVar7 = *(int *)(*piVar6 + iVar5 * 4);
      if ((((iVar7 != *(int *)(param_1 + 0x84)) && (iVar7 != *(int *)(param_1 + 0x88))) &&
          (iVar7 != *(int *)(param_1 + 0x90))) && (iVar7 != *(int *)(param_1 + 0x94))) {
        local_28 = iVar8;
      }
    }
    iVar7 = FUN_0001c550();
    if ((iVar7 != 0) && (*(int *)(param_1 + 0x34) != 0)) {
      iVar7 = FUN_0001c530();
      if ((iVar7 == 0) && (iVar7 = FUN_0001c530(), iVar7 == 0)) {
        do {
          iVar5 = (iVar5 + 1) % *(int *)(param_1 + 0x34);
        } while (*(int *)(*(int *)(*piVar6 + iVar5 * 4) + 0x2c) == 2);
      }
      else {
        do {
          iVar5 = iVar5 + -1;
          if (iVar5 < 0) {
            iVar5 = *(int *)(param_1 + 0x34) + -1;
          }
        } while (*(int *)(*(int *)(*piVar6 + iVar5 * 4) + 0x2c) == 2);
      }
      FUN_0001cb70();
      *(undefined4 *)(param_1 + 0x28) = 0;
    }
    iVar3 = 0;
    for (iVar7 = 0; iVar7 < *(int *)(param_1 + 0x76); iVar7 = iVar7 + 1) {
      iVar4 = FUN_0001c550();
      iVar7 = extraout_ECX_00;
      if (iVar4 == 0) {
        if (**(int **)(iVar3 + *(int *)(param_1 + 0x6e)) == 0) {
          iVar4 = FUN_0001c5a0();
          iVar7 = extraout_ECX_01;
          if (iVar4 != 0) goto LAB_00060782;
        }
      }
      else {
LAB_00060782:
        local_2c = *(int *)(*(int *)(iVar3 + *(int *)(param_1 + 0x6e)) + 4);
        local_28 = 1;
      }
      iVar3 = iVar3 + 4;
    }
    *(int *)(param_1 + 0x28) = *(int *)(param_1 + 0x28) + 1;
    if (iVar9 != 0) {
      iVar9 = iVar9 + -1;
    }
    if ((*(int *)(param_1 + 0x24) != 0) && (*(int *)(param_1 + 0x24) <= *(int *)(param_1 + 0x28))) {
      local_2c = *(int *)(param_1 + 0x20);
      local_28 = 1;
    }
    FUN_0005f310();
  } while( true );
}

