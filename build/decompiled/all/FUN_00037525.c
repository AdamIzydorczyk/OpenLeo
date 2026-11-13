
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_00037525(int *param_1)

{
  short *psVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined4 extraout_EDX;
  int extraout_EDX_00;
  int *piVar5;
  longdouble lVar6;
  longdouble lVar7;
  longdouble extraout_ST1;
  undefined4 uStack_6c;
  int local_68;
  int local_64;
  undefined **local_60;
  double local_5c;
  double local_54;
  double local_4c;
  double local_44;
  double local_3c;
  double local_2c;
  double local_24;
  int *local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
  local_1c = &local_68;
  local_60 = &PTR_LAB_00098278;
  uStack_6c = 0x37552;
  (*(code *)PTR_FUN_0009827c)();
  psVar1 = (short *)**(undefined4 **)(param_1[0xf] + 8);
  local_3c = (double)-((int)*psVar1 - (int)psVar1[2]);
  local_18 = -((int)psVar1[1] - (int)psVar1[3]);
  local_2c = (double)local_18;
  local_54 = local_3c * local_3c;
  local_4c = local_2c * local_2c;
  local_24 = local_54 + local_4c;
  uStack_6c = 0x375b5;
  lVar6 = (longdouble)FUN_00076ce8();
  local_5c = (double)lVar6;
  if (1.0 < local_5c) {
    uStack_6c = 0x375ca;
    lVar6 = (longdouble)FUN_00076ce8();
    local_44 = (double)lVar6;
    lVar7 = (longdouble)local_5c;
    lVar6 = (longdouble)_DAT_000911c1;
    do {
      lVar7 = lVar7 + lVar6;
      uStack_6c = 0x375ed;
      lVar6 = (longdouble)FUN_00075d96();
      param_1[0x10] = (int)ROUND(lVar6);
      lVar6 = extraout_ST1;
    } while ((longdouble)1 < lVar7);
    local_5c = (double)lVar7;
  }
  piVar5 = &local_68;
  uStack_6c = 0x3760b;
  lVar6 = (longdouble)FUN_00075d96();
  local_8 = (int)ROUND(lVar6);
  if (local_8 < 0) {
    uStack_6c = 0x37621;
    piVar3 = (int *)FUN_000229c0();
    local_8 = local_8 + *(short *)(*piVar3 + 0x17);
  }
  if (1.0 <= ABS(local_2c)) {
    uStack_6c = 0x3764e;
    FUN_000229c0();
    local_18 = -local_68;
    uStack_6c = 0x3767d;
    lVar6 = (longdouble)FUN_00075d96();
    local_18 = (int)ROUND(lVar6);
    uStack_6c = extraout_EDX;
    FUN_00057530();
    iVar2 = *(int *)(DAT_000cef40 + 0x390);
    iVar4 = FUN_00057500();
    local_10 = local_18 + (*(int *)(iVar2 + 0x10) - iVar4) + *param_1;
    FUN_000229c0();
    FUN_00042660();
    piVar5 = &uStack_6c;
  }
  *(undefined4 *)((int)piVar5 + -4) = 0x376f6;
  FUN_000229c0();
  local_18 = -local_64;
  *(undefined4 *)((int)piVar5 + -4) = 0x3771d;
  lVar6 = (longdouble)FUN_00075d96();
  iVar2 = local_8;
  local_18 = (int)ROUND(lVar6);
  local_c = -local_68;
  *(undefined4 *)((int)piVar5 + -4) = 0x3773d;
  lVar6 = (longdouble)FUN_00075d96();
  *(int *)((int)piVar5 + -4) = extraout_EDX_00 - iVar2;
  local_c = (int)ROUND(lVar6);
  *(undefined4 *)((int)piVar5 + -8) = 0x37759;
  FUN_00057530();
  iVar2 = *(int *)(DAT_000cef40 + 0x390);
  *(undefined4 *)((int)piVar5 + -8) = 0x37780;
  iVar4 = FUN_00057500();
  local_14 = local_c + (*(int *)(iVar2 + 0x10) - iVar4) + *param_1;
  *(undefined4 *)((int)piVar5 + -8) = 0x377a3;
  FUN_000229c0();
  *(undefined4 *)((int)piVar5 + -8) = 0x377ad;
  FUN_00042660();
  return;
}

