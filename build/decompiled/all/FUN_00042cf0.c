
int __regparm3
FUN_00042cf0(int param_1,byte *param_2,byte *param_3,int param_4,int *param_5,int *param_6)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int unaff_EBX;
  int iVar4;
  byte *pbVar5;
  int iVar6;
  int local_14;
  int local_10;
  
  iVar4 = 0;
  iVar1 = 0;
  if (0 < param_4) {
    pbVar5 = param_3;
    do {
      iVar6 = (param_1 - (uint)*param_2) - ((uint)*pbVar5 + unaff_EBX);
      if (0 < iVar6) {
        if (iVar4 == 0) {
          iVar4 = iVar6;
          local_14 = iVar1;
        }
        else if (iVar4 < iVar6) {
          iVar4 = iVar6;
        }
        local_10 = iVar1;
      }
      pbVar5 = pbVar5 + 1;
      iVar1 = iVar1 + 1;
      param_2 = param_2 + 1;
    } while (iVar1 < param_4);
  }
  if (iVar4 == 0) {
    *param_5 = 0;
    *param_6 = 0;
  }
  else {
    uVar2 = (uint)param_3[local_14] - (uint)param_3[local_10];
    iVar1 = (uVar2 ^ (int)uVar2 >> 0x1f) - ((int)uVar2 >> 0x1f);
    uVar3 = local_14 - local_10 >> 0x1f;
    iVar6 = (local_14 - local_10 ^ uVar3) - uVar3;
    if ((iVar6 == 0) || (iVar1 == 0)) {
      *param_5 = iVar4;
      *param_6 = 0;
    }
    else {
      iVar1 = iVar1 + iVar6;
      *param_5 = (iVar4 * iVar6) / iVar1;
      *param_6 = (int)(iVar6 * uVar2) / iVar1;
    }
  }
  return iVar4;
}

