
undefined4 __regparm3
FUN_00042a70(short *param_1,int *param_2,short *param_3,int *param_4,int param_5)

{
  int iVar1;
  int iVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  byte *pbVar6;
  int unaff_EBX;
  byte *pbVar7;
  int iVar8;
  byte *pbVar9;
  int iVar10;
  int local_30;
  int local_2c;
  int local_18;
  
  local_30 = *(int *)((int)param_1 + 0x27);
  if (local_30 == 0) {
    return 0;
  }
  if (*(int *)((int)param_3 + 0x27) == 0) {
    return 0;
  }
  if (unaff_EBX < 0) {
    if (*param_1 != 0x7fff) {
      param_2[1] = param_2[1] - (int)param_1[1];
      *param_2 = *param_2 -
                 (((int)*(short *)((int)param_1 + 0x15) + *(short *)((int)param_1 + 0x19) * 0x50) -
                 (int)*param_1);
    }
    local_30 = *(int *)((int)param_1 + 0x2f);
    iVar10 = *(int *)((int)param_1 + 0x27);
  }
  else {
    iVar10 = *(int *)((int)param_1 + 0x2f);
    if (*param_1 != 0x7fff) {
      param_2[1] = param_2[1] - (int)param_1[1];
      *param_2 = *param_2 - (int)*param_1;
    }
  }
  if (param_5 < 0) {
    if (*param_3 != 0x7fff) {
      param_4[1] = param_4[1] - (int)param_3[1];
      *param_4 = *param_4 -
                 ((*(short *)((int)param_3 + 0x19) * 0x50 + (int)*(short *)((int)param_3 + 0x15)) -
                 (int)*param_3);
    }
    iVar8 = *(int *)((int)param_3 + 0x2f);
    local_2c = *(int *)((int)param_3 + 0x27);
  }
  else {
    iVar8 = *(int *)((int)param_3 + 0x27);
    local_2c = *(int *)((int)param_3 + 0x2f);
    if (*param_3 != 0x7fff) {
      param_4[1] = param_4[1] - (int)param_3[1];
      *param_4 = *param_4 - (int)*param_3;
    }
  }
  iVar4 = param_2[1] - param_4[1];
  if (iVar4 < 0) {
    iVar2 = -iVar4;
    local_18 = 0;
    iVar5 = *(short *)((int)param_1 + 0x17) + iVar4;
    if (iVar5 < 1) {
      return 0;
    }
    iVar1 = (int)*(short *)((int)param_3 + 0x17);
    if (iVar5 <= *(short *)((int)param_3 + 0x17)) goto LAB_00042bdc;
  }
  else {
    iVar5 = *(short *)((int)param_3 + 0x17) - iVar4;
    iVar2 = 0;
    if (iVar5 < 1) {
      return 0;
    }
    iVar1 = (int)*(short *)((int)param_1 + 0x17);
    local_18 = iVar4;
    if (iVar5 <= *(short *)((int)param_1 + 0x17)) goto LAB_00042bdc;
  }
  iVar5 = iVar1;
LAB_00042bdc:
  iVar5 = iVar2 + iVar5;
  if (iVar2 < iVar5) {
    pbVar7 = (byte *)(local_30 + iVar2);
    pbVar6 = (byte *)(local_18 + iVar8);
    pbVar3 = (byte *)(local_18 + local_2c);
    pbVar9 = (byte *)(iVar2 + iVar10);
    do {
      if ((((*pbVar7 != 0xff) && (*pbVar6 != 0xff)) &&
          ((int)(*param_2 + (uint)*pbVar7) <
           (int)((*param_4 +
                 *(short *)((int)param_3 + 0x19) * 0x50 + (int)*(short *)((int)param_3 + 0x15)) -
                (uint)*pbVar3))) &&
         ((int)(*param_4 + (uint)*pbVar6) <
          (int)((*param_2 +
                (int)*(short *)((int)param_1 + 0x15) + *(short *)((int)param_1 + 0x19) * 0x50) -
               (uint)*pbVar9))) {
        return 1;
      }
      pbVar7 = pbVar7 + 1;
      pbVar9 = pbVar9 + 1;
      iVar2 = iVar2 + 1;
      pbVar6 = pbVar6 + 1;
      pbVar3 = pbVar3 + 1;
    } while (iVar2 < iVar5);
  }
  return 0;
}

