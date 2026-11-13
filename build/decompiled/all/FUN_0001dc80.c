
undefined8 __regparm2
FUN_0001dc80(undefined4 param_1,undefined4 param_2,byte *param_3,int param_4,int param_5,int param_6
            ,int param_7,int param_8)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  byte *pbVar6;
  byte *pbVar7;
  byte *pbVar8;
  
  if ((((0x13f < param_4) || (param_6 <= -param_4)) || (199 < param_5)) ||
     ((iVar2 = -param_5, param_8 <= iVar2 || (param_8 < 1)))) {
LAB_0001dda3:
    return CONCAT44(param_2,param_1);
  }
  if (param_7 != 0) {
    do {
      param_3 = param_3 + (*param_3 & 0xffffff7f) + 1;
      param_7 = param_7 + -1;
    } while (param_7 != 0);
  }
  iVar5 = param_8;
  if (0 < iVar2) {
    iVar5 = param_8 + param_5;
    if (iVar5 == 0 || param_8 < iVar2) goto LAB_0001dda3;
    do {
      param_3 = param_3 + (*param_3 & 0xffffff7f) + 1;
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    iVar2 = 0;
  }
  if (iVar2 + 200 <= iVar5) {
    iVar5 = iVar2 + 200;
  }
  pbVar7 = (byte *)(DAT_000cef2c + 400 + DAT_000cf0e8 + param_4 + iVar2 * -400);
LAB_0001dd3b:
  param_3 = param_3 + 1;
  uVar4 = (uint)*param_3;
  pbVar8 = pbVar7;
LAB_0001dd3f:
  pbVar8 = pbVar8 + uVar4;
  uVar4 = (uint)param_3[1];
  pbVar6 = param_3 + 2;
  if (0x7f < param_3[1]) goto LAB_0001dd66;
  do {
    for (uVar3 = uVar4 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(undefined4 *)pbVar8 = *(undefined4 *)pbVar6;
      pbVar6 = pbVar6 + 4;
      pbVar8 = pbVar8 + 4;
    }
    param_3 = pbVar6;
    for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pbVar8 = *param_3;
      param_3 = param_3 + 1;
      pbVar8 = pbVar8 + 1;
    }
    bVar1 = *param_3;
    uVar4 = (uint)bVar1;
    if (0x7f < bVar1) {
LAB_0001dd97:
      pbVar7 = pbVar7 + 400;
      iVar5 = iVar5 + -1;
      if (iVar5 == 0) goto LAB_0001dda3;
      goto LAB_0001dd3b;
    }
    if (bVar1 < 0x40) goto LAB_0001dd3f;
    pbVar6 = param_3 + 1;
    uVar4 = bVar1 & 0xffffff3f;
LAB_0001dd66:
    bVar1 = *pbVar6;
    for (uVar3 = (uVar4 & 0xffffff7f) >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
      *(uint *)pbVar8 = CONCAT22(CONCAT11(bVar1,bVar1),CONCAT11(bVar1,bVar1));
      pbVar8 = pbVar8 + 4;
    }
    for (uVar4 = uVar4 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
      *pbVar8 = bVar1;
      pbVar8 = pbVar8 + 1;
    }
    param_3 = pbVar6 + 1;
    bVar1 = *param_3;
    uVar4 = (uint)bVar1;
    if (0x7f < bVar1) goto LAB_0001dd97;
    if (bVar1 < 0x40) goto LAB_0001dd3f;
    pbVar6 = pbVar6 + 2;
    uVar4 = bVar1 & 0xffffff3f;
  } while( true );
}

