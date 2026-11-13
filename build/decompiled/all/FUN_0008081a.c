
longdouble __regparm3 FUN_0008081a(byte *param_1,undefined4 *param_2)

{
  byte bVar1;
  bool bVar2;
  bool bVar3;
  byte *pbVar4;
  byte *pbVar5;
  byte bVar6;
  byte extraout_CL;
  byte extraout_CL_00;
  byte extraout_CL_01;
  ushort uVar7;
  undefined4 extraout_ECX;
  byte bVar9;
  uint uVar8;
  int iVar10;
  int iVar11;
  longdouble lVar12;
  undefined4 local_40;
  byte local_3c [20];
  undefined8 local_28;
  int local_20;
  byte *local_1c;
  byte *local_18;
  
  for (pbVar4 = param_1; (bVar9 = *pbVar4, bVar9 == 0x20 || ((8 < bVar9 && (bVar9 < 0xe))));
      pbVar4 = pbVar4 + 1) {
  }
  if ((bVar9 == 0x2b) || (bVar9 == 0x2d)) {
    pbVar4 = pbVar4 + 1;
  }
  bVar6 = 0;
  iVar10 = 0;
  bVar9 = 0x30;
  local_20 = 0;
  local_18 = param_1;
  while( true ) {
    while( true ) {
      bVar1 = *pbVar4;
      uVar7 = CONCAT11(bVar6,bVar6) & 0x8ff;
      bVar3 = false;
      pbVar5 = pbVar4 + 1;
      bVar6 = (byte)uVar7;
      if (bVar1 != 0x2e) break;
      if (uVar7 >> 8 != 0) goto LAB_000808a7;
      bVar6 = bVar6 | 8;
      pbVar4 = pbVar5;
    }
    if ((bVar1 < 0x30) || (0x39 < bVar1)) break;
    if (uVar7 >> 8 != 0) {
      local_20 = local_20 + 1;
    }
    bVar9 = bVar9 | bVar1;
    if (bVar9 != 0x30) {
      if (iVar10 < 0x11) {
        local_3c[iVar10] = bVar1;
      }
      iVar10 = iVar10 + 1;
    }
    bVar6 = bVar6 | 4;
    pbVar4 = pbVar5;
  }
LAB_000808a7:
  iVar11 = 0;
  if ((CONCAT11(bVar6,bVar1) & 0x4ff) >> 8 != 0) {
    if ((bVar1 == 0x65) || (bVar1 == 0x45)) {
      if (*pbVar5 == 0x2b) {
LAB_000808eb:
        pbVar5 = pbVar4 + 2;
      }
      else if (*pbVar5 == 0x2d) {
        bVar3 = true;
        goto LAB_000808eb;
      }
      bVar2 = false;
      for (; (uVar8 = (uint)*pbVar5, 0x2f < uVar8 && (uVar8 < 0x3a)); pbVar5 = pbVar5 + 1) {
        if (iVar11 < 1000) {
          iVar11 = uVar8 + iVar11 * 10 + -0x30;
        }
        bVar2 = true;
      }
      if (bVar3) {
        iVar11 = -iVar11;
      }
      local_18 = pbVar5;
      local_1c = pbVar4;
      if (!bVar2) {
        local_18 = pbVar4;
      }
    }
    else {
      local_18 = pbVar4;
    }
  }
  iVar11 = iVar11 - local_20;
  if (0x11 < iVar10) {
    iVar11 = iVar11 + iVar10 + -0x11;
    iVar10 = 0x11;
  }
  for (; (0 < iVar10 && (local_3c[iVar10 + -1] == 0x30)); iVar10 = iVar10 + -1) {
    iVar11 = iVar11 + 1;
  }
  local_40 = param_2;
  if (iVar10 == 0) {
    local_28 = 0.0;
    goto LAB_000809f5;
  }
  local_3c[iVar10] = 0;
  FUN_0008175c();
  iVar10 = iVar11 + -1 + iVar10;
  if (iVar10 < 0x135) {
    if (iVar10 < -0x134) {
      FUN_00078c1e();
      local_28 = (double)CONCAT44(extraout_ECX,extraout_ECX);
      goto LAB_000809f5;
    }
    bVar9 = extraout_CL;
    if (iVar11 != 0) {
      lVar12 = (longdouble)FUN_000813d2((undefined4)local_28,local_28._4_4_,iVar11);
      local_28 = (double)lVar12;
      bVar9 = extraout_CL_01;
    }
    if ((bVar9 & 1) == 0) goto LAB_000809f5;
  }
  else {
    FUN_00078c1e();
    if ((extraout_CL_00 & 1) == 0) {
      local_28 = (double)CONCAT44(DAT_000a6bc8,DAT_000a6bc4);
      goto LAB_000809f5;
    }
    local_28 = (double)CONCAT44(DAT_000a6bc8,DAT_000a6bc4);
  }
  local_28 = -local_28;
LAB_000809f5:
  if (local_40 != (undefined4 *)0x0) {
    *local_40 = local_18;
  }
  return (longdouble)local_28;
}

