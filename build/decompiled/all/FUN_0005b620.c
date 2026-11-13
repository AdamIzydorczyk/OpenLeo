
uint __regparm2 FUN_0005b620(code *param_1,code *param_2,int param_3,int *param_4,int *param_5)

{
  int iVar1;
  int iVar2;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar3;
  char extraout_DH;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  bool bVar8;
  bool bVar9;
  int local_2c;
  int local_18;
  char local_10;
  
  bVar8 = false;
  local_2c = 1;
  local_18 = 0;
  local_10 = '\0';
  iVar4 = 0;
  uVar7 = 0;
LAB_0005b652:
  do {
    iVar2 = iVar4;
    iVar4 = iVar2 + 1;
    iVar1 = (*param_1)();
    if (extraout_DH == '\0') {
      if (((-1 < iVar1) && (iVar1 < 0x80)) && (((&DAT_000a68b4)[(byte)((char)iVar1 + 1)] & 2) != 0))
      goto LAB_0005b652;
      local_10 = '\x01';
    }
    iVar5 = param_3 + -1;
    if (iVar5 < 0) goto LAB_0005b7a0;
    if ((local_10 != '\x01') || ((local_10 = '\x02', iVar1 != 0x2b && (iVar1 != 0x2d)))) {
      uVar3 = extraout_ECX;
      if (local_10 == '\x02') {
        local_10 = '\x03';
        if ((iVar1 == 0x30) && ((local_18 = 1, extraout_ECX == 0 || (extraout_ECX == 0x10)))) {
          iVar4 = iVar2 + 2;
          iVar1 = (*param_1)();
          param_3 = param_3 + -2;
          if (param_3 < 0) goto LAB_0005b7a0;
          if ((iVar1 == 0x78) || (iVar1 == 0x58)) goto LAB_0005b652;
          uVar3 = extraout_ECX_00;
          iVar5 = param_3;
          if (extraout_ECX_00 == 0) {
            uVar3 = 8;
          }
        }
        if (uVar3 == 0) {
          uVar3 = 10;
        }
        else if (((int)uVar3 < 1) || (0x24 < (int)uVar3)) goto LAB_0005b7a0;
      }
      param_3 = iVar5;
      if (local_10 == '\x03') {
        if ((iVar1 < 0x30) || (0x39 < iVar1)) {
          if ((iVar1 < 0x61) || (0x7a < iVar1)) {
            if ((iVar1 < 0x41) || (0x5a < iVar1)) goto LAB_0005b7a0;
            iVar2 = iVar1 + -0x37;
          }
          else {
            iVar2 = iVar1 + -0x57;
          }
        }
        else {
          iVar2 = iVar1 + -0x30;
        }
        if ((int)uVar3 <= iVar2) goto LAB_0005b7a0;
        local_18 = local_18 + 1;
        uVar6 = uVar7 * uVar3 + iVar2;
        bVar9 = (uVar6 - iVar2) / uVar3 != uVar7;
        uVar7 = uVar6;
        if (bVar9) {
          local_2c = 2;
          uVar7 = bVar8 + 0x7fffffff;
LAB_0005b7a0:
          if (local_2c != 2) {
            (*param_2)();
            iVar4 = iVar4 + -1;
            if (bVar8) {
              uVar7 = -uVar7;
            }
          }
          if (local_18 == 0) {
            local_2c = (iVar1 != -1) - 1;
          }
          *param_5 = local_2c;
          *param_4 = *param_4 + iVar4;
          return uVar7;
        }
      }
      goto LAB_0005b652;
    }
    bVar8 = iVar1 == 0x2d;
    param_3 = iVar5;
  } while( true );
}

