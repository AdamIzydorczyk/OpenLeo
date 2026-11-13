
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm3 FUN_000185c0(undefined4 param_1,undefined4 param_2)

{
  byte bVar1;
  short sVar2;
  uint uVar3;
  uint uVar4;
  byte *unaff_ESI;
  byte *pbVar5;
  byte *pbVar6;
  byte *pbVar7;
  
  uVar4 = DAT_000c40a4;
  pbVar6 = DAT_000c3fdc;
  _DAT_000cde44 = _DAT_000cde44 + 1;
  if (DAT_000c40a4 == 0) {
    pbVar7 = DAT_000c3fdc;
    for (uVar4 = DAT_000c4088; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pbVar7 = 0x80;
      pbVar7 = pbVar7 + 1;
    }
  }
  else {
    if ((int)(DAT_000c40a4 - DAT_000c4088) < 0) {
      DAT_000c40a4 = 0;
      pbVar7 = DAT_000c3fdc;
      for (uVar3 = DAT_000c4088; uVar3 != 0; uVar3 = uVar3 - 1) {
        *pbVar7 = 0x80;
        pbVar7 = pbVar7 + 1;
      }
    }
    else {
      DAT_000c40a4 = DAT_000c40a4 - DAT_000c4088;
      uVar4 = DAT_000c4088;
    }
    if (DAT_000c408c == 1) {
      uVar4 = uVar4 >> 1;
      pbVar5 = DAT_000c40a0;
      do {
        DAT_000c40a0 = pbVar5 + 2;
        pbVar7 = pbVar6 + 1;
        *pbVar6 = (char)((ushort)*(undefined2 *)pbVar5 >> 8) + 0x80;
        uVar4 = uVar4 - 1;
        pbVar5 = DAT_000c40a0;
        pbVar6 = pbVar7;
      } while (uVar4 != 0);
    }
    else if (DAT_000c4090 == 1) {
      uVar4 = uVar4 >> 2;
      do {
        pbVar5 = DAT_000c40a0 + 2;
        sVar2 = *(short *)DAT_000c40a0;
        DAT_000c40a0 = DAT_000c40a0 + 4;
        pbVar7 = pbVar6 + 1;
        *pbVar6 = (char)((uint)((int)*(short *)pbVar5 + (int)sVar2) >> 9) + 0x80;
        uVar4 = uVar4 - 1;
        pbVar6 = pbVar7;
      } while (uVar4 != 0);
    }
    else if (DAT_000c4094 == 1) {
      uVar4 = uVar4 >> 1;
      pbVar5 = DAT_000c40a0;
      do {
        DAT_000c40a0 = pbVar5 + 2;
        pbVar7 = pbVar6 + 1;
        *pbVar6 = (char)((ushort)*(undefined2 *)pbVar5 >> 8) + 0x80;
        uVar4 = uVar4 - 1;
        pbVar5 = DAT_000c40a0;
        pbVar6 = pbVar7;
      } while (uVar4 != 0);
    }
    else {
      pbVar7 = pbVar6;
      if (DAT_000c4098 == 1) {
        uVar4 = uVar4 >> 1;
        do {
          pbVar5 = DAT_000c40a0 + 1;
          bVar1 = *DAT_000c40a0;
          DAT_000c40a0 = DAT_000c40a0 + 2;
          pbVar7 = pbVar6 + 1;
          *pbVar6 = (byte)((ushort)((ushort)*pbVar5 + (ushort)bVar1) >> 1);
          uVar4 = uVar4 - 1;
          pbVar6 = pbVar7;
        } while (uVar4 != 0);
      }
      else {
        for (; uVar4 != 0; uVar4 = uVar4 - 1) {
          *pbVar7 = *DAT_000c40a0;
          DAT_000c40a0 = DAT_000c40a0 + 1;
          pbVar7 = pbVar7 + 1;
        }
      }
    }
    unaff_ESI = DAT_000c40a0;
  }
  out(0x20,0x20);
  if (DAT_000c410c == 0) {
    if (DAT_000a88b0 == '\0') {
      DAT_000c410c = DAT_000c41d0 - 1;
      DAT_000a88b0 = 1;
      FUN_0001b1a0(pbVar7,unaff_ESI);
      DAT_000c41d2 = DAT_000c41d2 + 1;
      DAT_000a88b0 = '\0';
    }
  }
  else {
    DAT_000c410c = DAT_000c410c + -1;
  }
  return CONCAT44(param_2,param_1);
}

