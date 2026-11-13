
void __regparm3 FUN_000420b0(short *param_1)

{
  short sVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  short sVar5;
  char *pcVar6;
  uint uVar7;
  uint uVar8;
  
  for (iVar2 = 0;
      (iVar2 < *(short *)((int)param_1 + 0x17) &&
      (*(char *)(*(int *)((int)param_1 + 0x27) + iVar2) == -1)); iVar2 = iVar2 + 1) {
  }
  uVar4 = (int)*(short *)((int)param_1 + 0x15) + *(short *)((int)param_1 + 0x19) * 0x50;
  for (iVar3 = iVar2; iVar3 < *(short *)((int)param_1 + 0x17); iVar3 = iVar3 + 1) {
    uVar8 = (uint)*(byte *)(*(int *)((int)param_1 + 0x27) + iVar3);
    if ((int)uVar8 < (int)uVar4) {
      uVar4 = uVar8;
    }
  }
  iVar3 = *(short *)((int)param_1 + 0x17) + -1;
  if (-1 < iVar3) {
    pcVar6 = (char *)(*(int *)((int)param_1 + 0x2f) + iVar3);
    do {
      if (*pcVar6 != -1) break;
      iVar3 = iVar3 + -1;
      pcVar6 = pcVar6 + -1;
    } while (-1 < iVar3);
  }
  sVar1 = (short)iVar3;
  uVar8 = *(short *)((int)param_1 + 0x19) * 0x50 + (int)*(short *)((int)param_1 + 0x15);
  for (; -1 < iVar3; iVar3 = iVar3 + -1) {
    uVar7 = (uint)*(byte *)(*(int *)((int)param_1 + 0x2f) + iVar3);
    if ((int)uVar7 < (int)uVar8) {
      uVar8 = uVar7;
    }
  }
  sVar5 = (short)iVar2;
  param_1[9] = (sVar1 - sVar5) + 2;
  param_1[8] = (((*(short *)((int)param_1 + 0x15) + *(short *)((int)param_1 + 0x19) * 0x50) -
                (short)uVar8) - (short)uVar4) + 1;
  if (*param_1 != 0x7fff) {
    sVar5 = sVar5 - param_1[1];
  }
  param_1[7] = sVar5;
  if (*param_1 != 0x7fff) {
    uVar4 = uVar4 - (int)*param_1;
  }
  param_1[6] = (short)uVar4;
  return;
}

