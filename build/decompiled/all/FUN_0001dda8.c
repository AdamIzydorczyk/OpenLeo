
undefined8 __regparm2
FUN_0001dda8(undefined4 param_1,undefined4 param_2,uint *param_3,int param_4,int param_5,int param_6
            ,int param_7,int param_8)

{
  byte bVar1;
  int iVar2;
  short sVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint *puVar7;
  byte *pbVar8;
  byte *pbVar9;
  
  if ((((0x13f < param_4) || (param_6 <= -param_4)) || (199 < param_5)) ||
     ((iVar2 = -param_5, param_8 <= iVar2 || (param_8 == 0)))) {
LAB_0001decf:
    return CONCAT44(param_2,param_1);
  }
  if (param_7 != 0) {
    do {
      param_3 = (uint *)((int)param_3 + ((byte)*param_3 & 0xffffff7f) + 1);
      param_7 = param_7 + -1;
    } while (param_7 != 0);
  }
  iVar6 = param_8;
  if (0 < iVar2) {
    iVar6 = param_8 + param_5;
    if (iVar6 == 0 || param_8 < iVar2) goto LAB_0001decf;
    do {
      param_3 = (uint *)((int)param_3 + ((byte)*param_3 & 0xffffff7f) + 1);
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    iVar2 = 0;
  }
  if (-(200 - iVar6) != iVar2 && 200 - iVar6 <= -iVar2) {
    iVar6 = iVar2 + 200;
  }
  pbVar8 = (byte *)(DAT_000cef2c + DAT_000cf0e8 + param_4 + iVar2 * -400 + param_6 + 399);
  uVar4 = 0;
LAB_0001de5e:
  param_3 = (uint *)((int)param_3 + 1);
  uVar5 = (uint)*(byte *)param_3;
  pbVar9 = pbVar8;
LAB_0001de62:
  pbVar9 = pbVar9 + -uVar5;
  uVar4 = CONCAT31((int3)(uVar4 >> 8),*(byte *)((int)param_3 + 1));
  puVar7 = (uint *)((int)param_3 + 2);
  if (0x7f < *(byte *)((int)param_3 + 1)) goto LAB_0001deaa;
  do {
    uVar5 = uVar4 >> 2 | uVar4 << 0x1e;
    param_3 = puVar7;
    if ((char)(uVar4 >> 2) != '\0') {
      do {
        uVar4 = *puVar7;
        puVar7 = puVar7 + 1;
        *(uint *)(pbVar9 + -3) =
             uVar4 >> 0x18 | (uVar4 & 0xff0000) >> 8 | (uVar4 & 0xff00) << 8 | uVar4 << 0x18;
        pbVar9 = pbVar9 + -4;
        sVar3 = (short)uVar5 + -1;
        uVar5 = CONCAT22((short)(uVar5 >> 0x10),sVar3);
        param_3 = puVar7;
      } while (sVar3 != 0);
    }
    uVar5 = uVar5 << 2 | uVar5 >> 0x1e;
    uVar4 = uVar5;
    if ((char)uVar5 != '\0') {
      do {
        uVar4 = *param_3;
        param_3 = (uint *)((int)param_3 + 1);
        *pbVar9 = (byte)uVar4;
        pbVar9 = pbVar9 + -1;
        uVar5 = uVar5 - 1;
        uVar4 = 0;
      } while (uVar5 != 0);
    }
    bVar1 = (byte)*param_3;
    uVar5 = (uint)bVar1;
    if (0x7f < bVar1) {
LAB_0001dec3:
      pbVar8 = pbVar8 + 400;
      iVar6 = iVar6 + -1;
      if (iVar6 == 0) goto LAB_0001decf;
      goto LAB_0001de5e;
    }
    if (bVar1 < 0x40) goto LAB_0001de62;
    puVar7 = (uint *)((int)param_3 + 1);
    uVar4 = CONCAT31((int3)(uVar4 >> 8),bVar1) & 0xffffff3f;
LAB_0001deaa:
    uVar5 = *puVar7;
    for (uVar4 = uVar4 & 0xffffff7f; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pbVar9 = (byte)uVar5;
      pbVar9 = pbVar9 + -1;
    }
    param_3 = (uint *)((int)puVar7 + 1);
    bVar1 = *(byte *)param_3;
    uVar5 = (uint)bVar1;
    uVar4 = 0;
    if (0x7f < bVar1) goto LAB_0001dec3;
    if (bVar1 < 0x40) goto LAB_0001de62;
    puVar7 = (uint *)((int)puVar7 + 2);
    uVar4 = bVar1 & 0xffffff3f;
  } while( true );
}

