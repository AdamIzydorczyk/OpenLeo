
undefined8 __regparm2
FUN_0001d9b8(undefined4 param_1,undefined4 param_2,byte *param_3,int param_4,int param_5,int param_6
            ,int param_7,int param_8)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  byte *pbVar5;
  
  if ((((param_4 < 0x140) && (-param_4 < param_6)) && (param_5 < 200)) &&
     ((iVar2 = -param_5, iVar2 < param_8 && (0 < param_8)))) {
    if (param_7 != 0) {
      do {
        param_3 = param_3 + (*param_3 & 0xffffff7f) + 1;
        param_7 = param_7 + -1;
      } while (param_7 != 0);
    }
    iVar4 = param_8;
    if (0 < iVar2) {
      iVar4 = param_8 + param_5;
      if (iVar4 == 0 || param_8 < iVar2) goto LAB_0001da9f;
      do {
        param_3 = param_3 + (*param_3 & 0xffffff7f) + 1;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      iVar2 = 0;
    }
    if (iVar2 + 200 <= iVar4) {
      iVar4 = iVar2 + 200;
    }
    pbVar5 = (byte *)(DAT_000cef2c + 400 + DAT_000cf0e8 + param_4 + iVar2 * -400);
    param_3 = param_3 + 1;
    do {
      bVar1 = *param_3;
      do {
        pbVar5 = pbVar5 + bVar1;
        bVar1 = param_3[1];
        param_3 = param_3 + 2;
        if (0x7f < bVar1) goto LAB_0001da9a;
        for (uVar3 = (uint)(bVar1 >> 2); uVar3 != 0; uVar3 = uVar3 - 1) {
          *(undefined4 *)pbVar5 = *(undefined4 *)param_3;
          param_3 = param_3 + 4;
          pbVar5 = pbVar5 + 4;
        }
        for (uVar3 = bVar1 & 0xffffff03; uVar3 != 0; uVar3 = uVar3 - 1) {
          *pbVar5 = *param_3;
          param_3 = param_3 + 1;
          pbVar5 = pbVar5 + 1;
        }
        bVar1 = *param_3;
      } while (bVar1 < 0x80);
      param_3 = param_3 + 1;
LAB_0001da9a:
      pbVar5 = pbVar5 + (400 - param_6);
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
LAB_0001da9f:
  return CONCAT44(param_2,param_1);
}

