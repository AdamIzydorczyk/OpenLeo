
uint __regparm3 FUN_0007e61b(uint *param_1)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  byte bVar4;
  ushort uVar5;
  ushort uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  bool bVar10;
  
  uVar5 = (ushort)param_1[2];
  uVar7 = param_1[1];
  uVar2 = *param_1;
  uVar9 = 0xfffff800;
  if ((int)(uVar2 << 0x15) < 0) {
    if ((uVar2 & 0x3ff) == 0) {
      uVar9 = 0xfffff000;
    }
    bVar10 = 0xfffff7ff < uVar2;
    uVar2 = uVar2 + 0x800;
    bVar1 = CARRY4(uVar7,(uint)bVar10);
    uVar7 = uVar7 + bVar10;
    if (bVar1) {
      uVar7 = 0x80000000;
      uVar5 = uVar5 + 1;
    }
  }
  uVar2 = uVar2 & uVar9;
  uVar5 = uVar5 & 0x7fff;
  uVar6 = uVar5 + 0xc400;
  if (uVar6 < 0x7ff) {
    if (uVar5 == 0x3c00) {
      uVar7 = uVar2 >> 0xc | uVar7 << 0x14;
    }
    else {
      uVar7 = uVar2 >> 0xb | uVar7 << 0x15;
    }
  }
  else if (uVar6 < 0xc400) {
    uVar7 = uVar2 >> 0xb | uVar7 << 0x15;
    if (uVar6 != 0x43ff) {
      FUN_0007c9a4();
    }
  }
  else if ((short)uVar6 < -0x34) {
    uVar7 = 0;
  }
  else {
    bVar4 = -((char)uVar5 + -0xc);
    uVar3 = uVar2;
    uVar8 = uVar7;
    if (0x1f < bVar4) {
      bVar4 = bVar4 - 0x20;
      uVar8 = 0;
      uVar3 = uVar7;
      uVar9 = uVar2;
    }
    uVar7 = uVar9 >> (bVar4 & 0x1f) | uVar3 << 0x20 - (bVar4 & 0x1f);
    uVar7 = (uVar3 >> (bVar4 & 0x1f) | uVar8 << 0x20 - (bVar4 & 0x1f)) + (uint)CARRY4(uVar7,uVar7);
  }
  return uVar7;
}

