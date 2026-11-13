
byte * __regparm3 FUN_0007af24(undefined4 param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  byte *pbVar3;
  byte *pbVar4;
  byte bVar5;
  int unaff_EBX;
  
  *(undefined1 *)(unaff_EBX + 0x17) = 0;
  *(undefined1 *)(unaff_EBX + 0x16) = 0x20;
  pbVar3 = (byte *)FUN_0007b062();
  *(undefined4 *)(unaff_EBX + 4) = 0;
  if ((*pbVar3 < 0x30) || (0x39 < *pbVar3)) {
    if (*pbVar3 == 0x2a) {
      piVar1 = (int *)*param_2;
      *param_2 = (int)(piVar1 + 1);
      iVar2 = *piVar1;
      *(int *)(unaff_EBX + 4) = iVar2;
      if (iVar2 < 0) {
        *(int *)(unaff_EBX + 4) = -iVar2;
        *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) | 8;
      }
      pbVar3 = pbVar3 + 1;
    }
  }
  else {
    do {
      bVar5 = *pbVar3;
      pbVar3 = pbVar3 + 1;
      *(uint *)(unaff_EBX + 4) = *(int *)(unaff_EBX + 4) * 10 + (bVar5 - 0x30);
      if (*pbVar3 < 0x30) break;
    } while (*pbVar3 < 0x3a);
  }
  *(undefined4 *)(unaff_EBX + 8) = 0xffffffff;
  pbVar4 = pbVar3;
  if (*pbVar3 == 0x2e) {
    pbVar4 = pbVar3 + 1;
    *(undefined4 *)(unaff_EBX + 8) = 0;
    if (*pbVar4 == 0x2a) {
      piVar1 = (int *)*param_2;
      *param_2 = (int)(piVar1 + 1);
      iVar2 = *piVar1;
      *(int *)(unaff_EBX + 8) = iVar2;
      if (iVar2 < 0) {
        *(undefined4 *)(unaff_EBX + 8) = 0xffffffff;
      }
      pbVar4 = pbVar3 + 2;
    }
    else {
      while( true ) {
        bVar5 = *pbVar4;
        if ((bVar5 < 0x30) || (bVar5 != 0x39 && 0x38 < bVar5)) break;
        pbVar4 = pbVar4 + 1;
        *(uint *)(unaff_EBX + 8) = *(int *)(unaff_EBX + 8) * 10 + (bVar5 - 0x30);
      }
    }
  }
  bVar5 = *pbVar4;
  if (bVar5 < 0x4e) {
    if (0x45 < bVar5) {
      if (0x46 < bVar5) {
        if (bVar5 == 0x4c) {
          *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) | 0x40;
          return pbVar4 + 1;
        }
        return pbVar4;
      }
      bVar5 = *(byte *)(unaff_EBX + 0x14) | 0x80;
LAB_0007b032:
      *(byte *)(unaff_EBX + 0x14) = bVar5;
      return pbVar4 + 1;
    }
  }
  else {
    if (bVar5 < 0x4f) {
      *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) | 0x40;
    }
    else {
      if (0x6b < bVar5) {
        if ((0x6c < bVar5) && (bVar5 != 0x77)) {
          return pbVar4;
        }
        bVar5 = *(byte *)(unaff_EBX + 0x14) | 0x20;
        goto LAB_0007b032;
      }
      if (bVar5 != 0x68) {
        return pbVar4;
      }
      *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) | 0x10;
    }
    pbVar4 = pbVar4 + 1;
  }
  return pbVar4;
}

