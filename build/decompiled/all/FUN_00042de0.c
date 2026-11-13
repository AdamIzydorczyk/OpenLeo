
int __regparm3
FUN_00042de0(short *param_1,int *param_2,short *param_3,uint *param_4,int param_5,
            undefined4 *param_6,int *param_7)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int unaff_EBX;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint *unaff_retaddr;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  
  *param_6 = 0;
  *param_7 = 0;
  local_14 = *(int *)((int)param_1 + 0x27);
  if (local_14 == 0) {
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
    local_14 = *(int *)((int)param_1 + 0x2f);
    local_1c = *(int *)((int)param_1 + 0x27);
  }
  else {
    local_1c = *(int *)((int)param_1 + 0x2f);
    if (*param_1 != 0x7fff) {
      param_2[1] = param_2[1] - (int)param_1[1];
      *param_2 = *param_2 - (int)*param_1;
    }
  }
  if (param_5 < 0) {
    if (*param_3 != 0x7fff) {
      param_4[1] = param_4[1] - (int)param_3[1];
      *param_4 = *param_4 -
                 (((int)*(short *)((int)param_3 + 0x15) + *(short *)((int)param_3 + 0x19) * 0x50) -
                 (int)*param_3);
    }
    local_20 = *(int *)((int)param_3 + 0x2f);
    local_18 = *(int *)((int)param_3 + 0x27);
  }
  else {
    local_20 = *(int *)((int)param_3 + 0x27);
    local_18 = *(int *)((int)param_3 + 0x2f);
    if (*param_3 != 0x7fff) {
      param_4[1] = param_4[1] - (int)param_3[1];
      *param_4 = *param_4 - (int)*param_3;
    }
  }
  iVar4 = param_2[1] - param_4[1];
  if (iVar4 < 0) {
    iVar6 = -iVar4;
    iVar5 = *(short *)((int)param_1 + 0x17) + iVar4;
    iVar4 = 0;
    if (iVar5 < 1) {
      return 0;
    }
    iVar1 = (int)*(short *)((int)param_3 + 0x17);
    if (iVar5 <= iVar1) goto LAB_00042f79;
  }
  else {
    iVar5 = *(short *)((int)param_3 + 0x17) - iVar4;
    iVar6 = 0;
    if (iVar5 < 1) {
      return 0;
    }
    iVar1 = (int)*(short *)((int)param_1 + 0x17);
    if (iVar5 <= iVar1) goto LAB_00042f79;
  }
  iVar5 = iVar1;
LAB_00042f79:
  uVar2 = *param_2 + *(short *)((int)param_1 + 0x19) * 0x50 + (int)*(short *)((int)param_1 + 0x15);
  iVar7 = iVar6 + iVar5 / 2;
  iVar1 = iVar5 / 2 + iVar4;
  if ((int)((((((*param_2 * 6 +
                 (*(short *)((int)param_1 + 0x19) * 0x50 + (int)*(short *)((int)param_1 + 0x15)) * 3
                + (uint)*(byte *)(local_14 + iVar6)) - (uint)*(byte *)(local_1c + iVar6)) +
              (uint)*(byte *)(iVar7 + local_14)) - (uint)*(byte *)(iVar7 + local_1c)) +
            (uint)*(byte *)(local_14 + iVar6 + iVar5 + -1)) -
           (uint)*(byte *)(local_1c + iVar6 + iVar5 + -1)) <
      (int)((((((*param_4 * 6 +
                 ((int)*(short *)((int)param_3 + 0x15) + *(short *)((int)param_3 + 0x19) * 0x50) * 3
                + (uint)*(byte *)(local_20 + iVar4)) - (uint)*(byte *)(local_18 + iVar4)) +
              (uint)*(byte *)(local_20 + iVar1)) - (uint)*(byte *)(local_18 + iVar1)) +
            (uint)*(byte *)(iVar4 + iVar5 + -1 + local_20)) -
           (uint)*(byte *)(iVar4 + iVar5 + local_18 + -1))) {
    iVar4 = FUN_00042cf0(iVar5,param_6,param_7);
  }
  else {
    iVar4 = FUN_00042cf0(iVar5,param_6,param_7);
    *unaff_retaddr = -*unaff_retaddr;
    iVar4 = -iVar4;
    *param_7 = -*param_7;
  }
  uVar3 = (int)*param_4 >> 0x1f;
  iVar5 = (*param_4 ^ uVar3) - uVar3;
  uVar3 = (int)*unaff_retaddr >> 0x1f;
  iVar6 = (*unaff_retaddr ^ uVar3) - uVar3;
  if (iVar5 < iVar6) {
    iVar5 = iVar6;
  }
  if ((int)((uVar2 ^ (int)uVar2 >> 0x1f) - ((int)uVar2 >> 0x1f)) < iVar5) {
    *unaff_retaddr = 0;
    *param_4 = uVar2;
  }
  return iVar4;
}

