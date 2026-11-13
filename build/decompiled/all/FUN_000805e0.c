
void __regparm2
FUN_000805e0(undefined4 param_1,undefined4 *param_2,int param_3,int param_4,undefined4 param_5,
            char param_6,byte param_7)

{
  double dVar1;
  int iVar2;
  undefined2 extraout_CX;
  uint uVar3;
  uint extraout_ECX;
  double *extraout_EDX;
  double *pdVar4;
  int unaff_EBX;
  undefined8 *puVar5;
  undefined4 *puVar6;
  int iVar7;
  longdouble lVar8;
  undefined8 uVar9;
  undefined8 uStack_58;
  undefined4 uStack_50;
  undefined4 uStack_4c;
  char *pcStack_48;
  char local_44 [20];
  double local_30;
  undefined8 local_28;
  int local_20;
  int local_1c;
  int local_18;
  char *local_14;
  char *local_10;
  
  pcStack_48 = local_44;
  uStack_4c = param_2[1];
  uStack_50 = *param_2;
  uStack_58._4_4_ = 0x80600;
  uVar9 = FUN_00081253();
  pdVar4 = (double *)((ulonglong)uVar9 >> 0x20);
  puVar5 = (undefined8 *)&uStack_50;
  puVar6 = &uStack_50;
  if ((int)uVar9 == 0) {
    uVar3 = (uint)param_7;
    if (param_7 == 0x47) {
      local_30 = ABS(*pdVar4);
      if ((((ulonglong)local_30 & 0x7fffffff00000000) != 0) ||
         (puVar5 = (undefined8 *)&uStack_50, local_30._0_4_ != 0)) {
        uStack_58._4_4_ = 0x8064f;
        lVar8 = (longdouble)thunk_FUN_000812e8();
        local_30 = (double)lVar8;
        uStack_58 = local_30;
        FUN_00081355();
        lVar8 = (longdouble)FUN_00075d96();
        puVar5 = &uStack_58;
        local_18 = (int)ROUND(lVar8);
        pdVar4 = extraout_EDX;
        if ((local_18 < -4) || (unaff_EBX <= local_18)) {
          uVar3 = 0x45;
          goto LAB_00080695;
        }
        if (0.0 <= local_30) {
          local_18 = local_18 + 1;
        }
        unaff_EBX = unaff_EBX - local_18;
        puVar5 = &uStack_58;
        if (local_18 < 0) {
          unaff_EBX = unaff_EBX + -1;
          puVar5 = &uStack_58;
        }
      }
      uVar3 = 0x46;
      param_4 = 0;
    }
LAB_00080695:
    if ((uVar3 == 0x45) && ((param_4 <= -unaff_EBX || (unaff_EBX + 2 <= param_4))))
    goto LAB_000806b0;
    if (param_7 == 0x45) {
      if (param_4 < 1) {
        if (param_4 < 0) {
          unaff_EBX = param_4 + unaff_EBX;
        }
      }
      else {
        unaff_EBX = unaff_EBX + 1;
      }
    }
    dVar1 = *pdVar4;
    local_28 = dVar1;
    if ((((((ulonglong)*pdVar4 & 0x7fffffff00000000) != 0) || (*(int *)pdVar4 != 0)) &&
        (uVar3 != 0x45)) && (param_4 != 0)) {
      *(int *)((int)puVar5 + -4) = param_4;
      local_28._4_4_ = (undefined4)((ulonglong)dVar1 >> 0x20);
      *(undefined4 *)((int)puVar5 + -8) = local_28._4_4_;
      local_28._0_4_ = SUB84(dVar1,0);
      *(int *)((int)puVar5 + -0xc) = (int)local_28;
      *(undefined4 *)((int)puVar5 + -0x10) = 0x80718;
      lVar8 = (longdouble)FUN_000813d2();
      puVar5 = (undefined8 *)((int)puVar5 + -0xc);
      local_28 = (double)lVar8;
      uVar3 = extraout_ECX;
    }
    *(char **)((int)puVar5 + -4) = local_44;
    *(uint *)((int)puVar5 + -8) = uVar3 & 0xff;
    *(int **)((int)puVar5 + -0xc) = &local_1c;
    *(int **)((int)puVar5 + -0x10) = &local_20;
    *(int *)((int)puVar5 + -0x14) = unaff_EBX;
    *(undefined4 *)((int)puVar5 + -0x18) = local_28._4_4_;
    *(int *)((int)puVar5 + -0x1c) = (int)local_28;
    *(undefined4 *)((int)puVar5 + -0x20) = 0x8073a;
    FUN_000814ef();
    local_14 = local_10;
    if (local_1c == 0) {
      if ((char)((ushort)extraout_CX >> 8) != '\0') {
        *local_10 = '+';
        local_14 = local_10 + 1;
      }
    }
    else {
      *local_10 = '-';
      local_14 = local_10 + 1;
    }
    iVar7 = param_3 + 1;
    if ((char)extraout_CX == 'E') {
      if (unaff_EBX - param_4 <= param_3) {
        *(undefined4 *)((int)puVar5 + -0x20) = 0x80788;
        local_14 = (char *)FUN_0008050c();
        iVar7 = (int)local_14 - (int)local_10;
        if ((param_6 != '\0') && (iVar7 < param_3)) {
          *local_14 = param_6;
          local_14 = local_14 + 1;
          iVar7 = iVar7 + 1;
        }
        if ((((ulonglong)local_28 & 0x7fffffff00000000) != 0) || ((int)local_28 != 0)) {
          local_20 = local_20 - param_4;
        }
        *(undefined4 *)((int)puVar5 + -0x20) = 0x807ce;
        iVar2 = FUN_00080469();
        puVar6 = (undefined4 *)((int)puVar5 + -0x1c);
        iVar7 = iVar7 + iVar2;
        goto LAB_000807fd;
      }
    }
    else if ((int)(local_14 + ((local_20 + 1 + unaff_EBX) - (int)local_10)) <= param_3) {
      *(undefined4 *)((int)puVar5 + -0x20) = 0x807f9;
      iVar7 = FUN_0008050c();
      iVar7 = iVar7 - (int)local_10;
    }
    puVar6 = (undefined4 *)((int)puVar5 + -0x1c);
  }
  else {
    for (iVar7 = 0; local_44[iVar7] != '\0'; iVar7 = iVar7 + 1) {
      if (iVar7 < param_3) {
        *local_10 = local_44[iVar7];
      }
      local_10 = local_10 + 1;
    }
  }
LAB_000807fd:
  puVar5 = (undefined8 *)puVar6;
  if (iVar7 <= param_3) {
    *(undefined4 *)((int)puVar6 + -4) = 0x80811;
    FUN_0008056f();
    return;
  }
LAB_000806b0:
  *(undefined4 *)((int)puVar5 + -4) = 0x806bf;
  FUN_000749a0();
  return;
}

