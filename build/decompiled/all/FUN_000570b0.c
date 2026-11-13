
void __regparm3 FUN_000570b0(short *param_1,int param_2,int param_3)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int unaff_EBX;
  int iVar4;
  int local_14;
  int local_10;
  
  local_10 = 200;
  local_14 = 0;
  if (param_3 < 0) {
    local_10 = param_3 + 200;
    local_14 = param_3 * -400;
    param_3 = 0;
  }
  if ((int)param_1[1] < local_10 + param_3) {
    local_10 = param_1[1] - param_3;
  }
  if (0 < local_10) {
    sVar1 = param_1[6];
    iVar4 = 0;
    do {
      iVar3 = param_2 % (int)*param_1;
      iVar2 = DAT_000cef2c + 400 + iVar4;
      param_2 = param_2 + 1;
      iVar4 = iVar4 + 0x50;
      FUN_0001d58c((iVar2 - unaff_EBX) + DAT_000cf0e8 + local_14,
                   *(int *)(param_1 + 2) +
                   *(int *)(*(int *)(param_1 + 9) + iVar3 * 4) +
                   *(int *)((param_3 / (int)sVar1) * 4 + *(int *)(*(int *)(param_1 + 7) + iVar3 * 4)
                           ),param_3 % (int)param_1[6],local_10);
    } while (iVar4 != 400);
  }
  return;
}

