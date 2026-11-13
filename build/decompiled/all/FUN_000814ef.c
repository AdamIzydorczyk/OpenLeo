
undefined8 __regparm2
FUN_000814ef(undefined4 param_1,undefined4 param_2,double param_3,int param_4,undefined4 *param_5,
            undefined4 *param_6,int param_7,char *param_8)

{
  char cVar1;
  undefined4 uVar2;
  undefined4 extraout_ECX;
  int *extraout_ECX_00;
  int *extraout_ECX_01;
  int *extraout_ECX_02;
  int *piVar3;
  int iVar4;
  int extraout_EDX;
  int *piVar5;
  char *pcVar6;
  longdouble lVar7;
  undefined8 uVar8;
  undefined4 uStack_88;
  undefined4 uStack_84;
  int iStack_80;
  short *psStack_7c;
  undefined4 uStack_78;
  undefined4 uStack_74;
  int iStack_70;
  short *psStack_6c;
  undefined4 uStack_68;
  undefined4 uStack_64;
  undefined1 *puStack_60;
  undefined4 uStack_5c;
  undefined4 uStack_58;
  undefined4 uStack_54;
  int iStack_50;
  undefined4 uStack_4c;
  char *pcStack_48;
  char local_44 [20];
  short local_30;
  short sStack_2e;
  short local_2c;
  short sStack_2a;
  undefined1 local_28 [8];
  int local_20;
  char local_1c;
  
  pcStack_48 = param_8;
  uStack_4c = param_3._4_4_;
  *param_6 = 0;
  iStack_50 = param_3._0_4_;
  *param_5 = 0;
  uStack_54 = 0x81520;
  uVar8 = FUN_00081253();
  uVar2 = (undefined4)((ulonglong)uVar8 >> 0x20);
  piVar5 = &iStack_50;
  if ((int)uVar8 == 0) {
    local_30 = 0;
    sStack_2e = 0;
    local_2c = 0;
    sStack_2a = 0;
    if ((((ulonglong)param_3 & 0x7fffffff00000000) != 0) || (param_3._0_4_ != 0)) {
      if (param_3 < 0.0) {
        param_3 = -param_3;
        *param_6 = 0xffffffff;
      }
      uStack_58 = param_3._4_4_;
      uStack_5c = param_3._0_4_;
      puStack_60 = (undefined1 *)0x8157b;
      uStack_54 = extraout_ECX;
      FUN_000824d8();
      iVar4 = *extraout_ECX_00;
      if (iVar4 < 0) {
        iVar4 = -iVar4;
      }
      iVar4 = (iVar4 * 3 + 5) / 10;
      if (*extraout_ECX_00 < 0) {
        iVar4 = -iVar4;
      }
      *extraout_ECX_00 = iVar4;
      if (param_7 == 0x46) {
        param_4 = param_4 + iVar4;
      }
      piVar3 = extraout_ECX_00;
      piVar5 = &uStack_5c;
      if (-1 < param_4) {
        if (0x10 < param_4) {
          param_4 = 0x10;
        }
        local_1c = '\0';
        puStack_60 = local_28;
        uStack_64 = param_3._4_4_;
        uStack_68 = param_3._0_4_;
        psStack_6c = (short *)0x815d9;
        lVar7 = (longdouble)FUN_00081bdc();
        if (((longdouble)0 == lVar7) && (extraout_EDX < param_4)) {
          local_1c = '\x01';
          param_4 = extraout_EDX;
        }
        psStack_6c = &local_30;
        uStack_74 = param_3._4_4_;
        uStack_78 = param_3._0_4_;
        psStack_7c = (short *)0x81601;
        iStack_70 = param_4;
        iVar4 = FUN_00081391();
        piVar3 = extraout_ECX_01;
        piVar5 = &uStack_78;
        if (iVar4 != 0) {
          *extraout_ECX_01 = *extraout_ECX_01 + iVar4;
          if ((param_7 == 0x46) || (local_1c != '\0')) {
            param_4 = param_4 + iVar4;
          }
          if (param_4 < 1) {
            param_4 = 1;
          }
          else if (0x10 < param_4) {
            param_4 = 0x10;
          }
          psStack_7c = &local_30;
          uStack_84 = param_3._4_4_;
          uStack_88 = param_3._0_4_;
          iStack_80 = param_4;
          iVar4 = FUN_00081391();
          piVar3 = extraout_ECX_02;
          piVar5 = &uStack_88;
          if (0 < iVar4) {
            *extraout_ECX_02 = *extraout_ECX_02 + 1;
            piVar5 = &uStack_88;
          }
        }
      }
      local_20 = (int)sStack_2e;
      if (((local_30 == 0 && sStack_2e == 0) && local_2c == 0) && sStack_2a == 0) {
        *param_6 = 0;
        *piVar3 = 0;
      }
    }
    pcVar6 = local_44;
    *(undefined4 *)((int)piVar5 + -4) = 0x816a4;
    FUN_00081c9b();
    *(char **)((int)piVar5 + -4) = param_8;
    do {
      cVar1 = *pcVar6;
      *param_8 = cVar1;
      if (cVar1 == '\0') break;
      cVar1 = pcVar6[1];
      pcVar6 = pcVar6 + 2;
      param_8[1] = cVar1;
      param_8 = param_8 + 2;
    } while (cVar1 != '\0');
    uVar2 = *(undefined4 *)((int)piVar5 + -4);
  }
  return CONCAT44(param_2,uVar2);
}

