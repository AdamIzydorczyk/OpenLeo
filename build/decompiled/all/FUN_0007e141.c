
longlong __regparm3 FUN_0007e141(uint param_1,uint param_2,uint param_3)

{
  uint uVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  longlong lVar4;
  int iVar5;
  uint uVar6;
  ushort uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint unaff_EBX;
  uint unaff_ESI;
  uint uVar11;
  uint uVar12;
  byte bVar13;
  ushort uVar14;
  ushort uVar15;
  bool bVar16;
  bool bVar17;
  bool bVar18;
  uint local_20;
  uint local_1c;
  
  uVar7 = (ushort)unaff_ESI;
  if (((param_3 == 0) && (unaff_EBX == 0)) && ((unaff_ESI & 0x7fff0000) == 0)) {
    if (((param_1 == 0) && (param_2 == 0)) && ((unaff_ESI & 0x7fff) == 0)) {
      FUN_0007c9a4();
      uVar10 = 0xc0000000;
    }
    else {
      FUN_0007c9a4();
      uVar10 = 0x80000000;
    }
    return (ulonglong)uVar10 << 0x20;
  }
  if ((param_1 == 0) && (param_2 == 0)) {
    if ((ushort)(uVar7 * 2) == 0) {
      return 0;
    }
    unaff_ESI = CONCAT22((short)(unaff_ESI >> 0x10),
                         (short)(CONCAT12(CARRY2(uVar7,uVar7),uVar7 * 2) >> 1));
  }
  uVar7 = (ushort)(unaff_ESI >> 0x10) & 0x7fff;
  uVar10 = (int)(unaff_ESI << 0x10 | unaff_ESI >> 0x10) >> 0x10 & 0x7fff;
  uVar14 = (ushort)uVar10;
  if (uVar14 == 0) {
    do {
      bVar16 = CARRY4(param_1,param_1);
      param_1 = param_1 * 2;
      param_2 = param_2 * 2 + (uint)bVar16;
      uVar14 = (short)uVar10 - 1;
      uVar10 = (uint)uVar14;
    } while (-1 < (int)param_2);
  }
  if ((unaff_ESI & 0x7fff0000) == 0) {
    do {
      bVar16 = CARRY4(unaff_EBX,unaff_EBX);
      unaff_EBX = unaff_EBX * 2;
      param_3 = param_3 * 2 + (uint)bVar16;
      uVar7 = uVar7 - 1;
    } while (-1 < (int)param_3);
  }
  uVar15 = (uVar14 - uVar7) + 0x3fff;
  if (((short)uVar15 < 0) || (uVar15 < 0x7fff)) {
    if ((short)uVar15 < -0x40) {
      local_20 = 0;
      uVar10 = 0;
    }
    else {
      bVar16 = param_3 <= param_2;
      uVar10 = param_2;
      if (bVar16) {
        uVar10 = param_2 - param_3;
      }
      uVar2 = CONCAT44(uVar10,param_1) / (ulonglong)param_3;
      local_1c = (uint)uVar2;
      uVar3 = (ulonglong)unaff_EBX * (uVar2 & 0xffffffff);
      iVar5 = (int)uVar3;
      lVar4 = (ulonglong)param_3 * (uVar2 & 0xffffffff) + (uVar3 >> 0x20);
      if (bVar16) {
        lVar4 = lVar4 + CONCAT44(param_3,unaff_EBX);
      }
      uVar8 = -iVar5;
      uVar10 = (uint)(iVar5 != 0);
      uVar1 = param_1 - (uint)lVar4;
      uVar11 = uVar1 - uVar10;
      for (iVar5 = (param_2 - (int)((ulonglong)lVar4 >> 0x20)) -
                   (uint)(param_1 < (uint)lVar4 || uVar1 < uVar10); iVar5 != 0;
          iVar5 = iVar5 + (uint)(bVar18 || CARRY4(uVar10,(uint)bVar17))) {
        bVar17 = local_1c == 0;
        local_1c = local_1c - 1;
        bVar16 = (bool)(bVar16 ^ bVar17);
        bVar17 = CARRY4(uVar8,unaff_EBX);
        uVar8 = uVar8 + unaff_EBX;
        bVar18 = CARRY4(uVar11,param_3);
        uVar10 = uVar11 + param_3;
        uVar11 = uVar10 + bVar17;
      }
      if (param_3 <= uVar11) {
        uVar11 = uVar11 - param_3;
        bVar17 = 0xfffffffe < local_1c;
        local_1c = local_1c + 1;
        bVar16 = (bool)(bVar16 ^ bVar17);
      }
      uVar2 = CONCAT44(uVar11,uVar8) / (ulonglong)param_3;
      local_20 = (uint)uVar2;
      if (local_20 != 0) {
        uVar3 = (ulonglong)unaff_EBX * (uVar2 & 0xffffffff);
        iVar5 = (int)uVar3;
        lVar4 = (ulonglong)param_3 * (uVar2 & 0xffffffff) + (uVar3 >> 0x20);
        uVar6 = (uint)lVar4;
        uVar9 = -iVar5;
        uVar10 = (uint)(iVar5 != 0);
        uVar1 = uVar8 - uVar6;
        uVar12 = uVar1 - uVar10;
        for (iVar5 = (uVar11 - (int)((ulonglong)lVar4 >> 0x20)) -
                     (uint)(uVar8 < uVar6 || uVar1 < uVar10); iVar5 != 0;
            iVar5 = iVar5 + (uint)(bVar18 || CARRY4(uVar10,(uint)bVar17))) {
          bVar18 = local_20 == 0;
          local_20 = local_20 - 1;
          bVar17 = local_1c < bVar18;
          local_1c = local_1c - bVar18;
          bVar16 = (bool)(bVar16 ^ bVar17);
          bVar17 = CARRY4(uVar9,unaff_EBX);
          uVar9 = uVar9 + unaff_EBX;
          bVar18 = CARRY4(uVar12,param_3);
          uVar10 = uVar12 + param_3;
          uVar12 = uVar10 + bVar17;
        }
      }
      uVar10 = local_1c;
      uVar7 = (uVar14 - uVar7) + 0x3ffe;
      if (bVar16) {
        uVar10 = (uint)(CONCAT14(1,local_1c) >> 1);
        local_20 = (uint)(CONCAT14((local_1c & 1) != 0,local_20) >> 1);
        uVar7 = uVar15;
      }
      if ((short)uVar7 < 1) {
        if (uVar7 == 0) {
          bVar13 = 1;
        }
        else {
          bVar13 = -(char)uVar7;
        }
        local_20 = local_20 >> (bVar13 & 0x1f) | uVar10 << 0x20 - (bVar13 & 0x1f);
        uVar10 = uVar10 >> (bVar13 & 0x1f) | 0 << 0x20 - (bVar13 & 0x1f);
      }
    }
  }
  else {
    uVar10 = 0x80000000;
    local_20 = 0;
  }
  return CONCAT44(uVar10,local_20);
}

