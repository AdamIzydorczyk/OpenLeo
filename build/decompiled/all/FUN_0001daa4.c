
undefined8 __regparm2
FUN_0001daa4(undefined4 param_1,undefined4 param_2,byte *param_3,int param_4,int param_5,int param_6
            ,int param_7,int param_8)

{
  byte *pbVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint *puVar7;
  byte *pbVar8;
  
  if ((((param_4 < 0x140) && (-param_4 < param_6)) && (param_5 < 200)) &&
     ((iVar3 = -param_5, iVar3 < param_8 && (param_8 != 0)))) {
    if (param_7 != 0) {
      do {
        param_3 = param_3 + (*param_3 & 0xffffff7f) + 1;
        param_7 = param_7 + -1;
      } while (param_7 != 0);
    }
    iVar6 = param_8;
    if (0 < iVar3) {
      iVar6 = param_8 + param_5;
      if (iVar6 == 0 || param_8 < iVar3) goto LAB_0001db9d;
      do {
        param_3 = param_3 + (*param_3 & 0xffffff7f) + 1;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      iVar3 = 0;
    }
    if (-(200 - iVar6) != iVar3 && 200 - iVar6 <= -iVar3) {
      iVar6 = iVar3 + 200;
    }
    pbVar8 = (byte *)(DAT_000cef2c + DAT_000cf0e8 + param_4 + iVar3 * -400 + param_6 + 399);
    uVar4 = 0;
    puVar7 = (uint *)(param_3 + 1);
    do {
      uVar4 = CONCAT31((int3)(uVar4 >> 8),(byte)*puVar7);
      do {
        pbVar8 = pbVar8 + -uVar4;
        pbVar1 = (byte *)((int)puVar7 + 1);
        uVar4 = CONCAT31((int3)(uVar4 >> 8),*pbVar1);
        puVar7 = (uint *)((int)puVar7 + 2);
        if (0x7f < *pbVar1) goto LAB_0001db98;
        uVar5 = uVar4 >> 2 | uVar4 << 0x1e;
        if ((char)(uVar4 >> 2) != '\0') {
          do {
            uVar4 = *puVar7;
            puVar7 = puVar7 + 1;
            *(uint *)(pbVar8 + -3) =
                 uVar4 >> 0x18 | (uVar4 & 0xff0000) >> 8 | (uVar4 & 0xff00) << 8 | uVar4 << 0x18;
            pbVar8 = pbVar8 + -4;
            sVar2 = (short)uVar5 + -1;
            uVar5 = CONCAT22((short)(uVar5 >> 0x10),sVar2);
          } while (sVar2 != 0);
        }
        uVar5 = uVar5 << 2 | uVar5 >> 0x1e;
        uVar4 = uVar5;
        if ((char)uVar5 != '\0') {
          do {
            uVar4 = *puVar7;
            puVar7 = (uint *)((int)puVar7 + 1);
            *pbVar8 = (byte)uVar4;
            pbVar8 = pbVar8 + -1;
            uVar5 = uVar5 - 1;
            uVar4 = 0;
          } while (uVar5 != 0);
        }
        uVar4 = CONCAT31((int3)(uVar4 >> 8),(byte)*puVar7);
      } while ((byte)*puVar7 < 0x80);
      puVar7 = (uint *)((int)puVar7 + 1);
LAB_0001db98:
      pbVar8 = pbVar8 + param_6 + 400;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
  }
LAB_0001db9d:
  return CONCAT44(param_2,param_1);
}

