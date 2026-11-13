
int __regparm3 FUN_0007e467(uint *param_1)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  ushort uVar4;
  short sVar5;
  byte bVar6;
  uint uVar7;
  bool bVar8;
  
  uVar4 = (ushort)param_1[2] & 0x7fff;
  sVar5 = uVar4 + 0xc002;
  if (uVar4 < 0x3ffe) {
    return 0;
  }
  if ((0x20 < sVar5) || (bVar6 = (byte)sVar5, '\x1f' < (char)bVar6)) {
    return -0x80000000;
  }
  uVar2 = param_1[1];
  bVar1 = (byte)((ushort)(CONCAT11(CARRY4(*param_1,*param_1),*(byte *)((int)param_1 + 9) >> 1) |
                         0x1f) >> 1);
  if (bVar6 == 0x20) {
    bVar8 = CARRY1(bVar1,bVar1);
    uVar7 = uVar2;
  }
  else {
    uVar7 = 0 << (bVar6 & 0x1f) | uVar2 >> 0x20 - (bVar6 & 0x1f);
    uVar2 = uVar2 << (bVar6 & 0x1f);
    bVar8 = CARRY4(uVar2,uVar2);
  }
  bVar6 = bVar1 * '\x02' & (byte)(CONCAT11(bVar8,0xff) >> 1);
  iVar3 = uVar7 + CARRY1(bVar6,bVar6);
  if (CARRY1(bVar6 * '\x02',bVar6 * '\x02')) {
    iVar3 = -iVar3;
  }
  return iVar3;
}

