
void __regparm3 FUN_00056c72(short *param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 unaff_EBX;
  int iVar4;
  int local_10;
  int local_c;
  int local_8;
  
  local_c = 200;
  local_10 = 0;
  local_8 = param_3;
  if (param_3 < 0) {
    local_c = param_3 + 200;
    local_10 = param_3 * -400;
    local_8 = 0;
  }
  if ((int)param_1[1] < local_c + local_8) {
    local_c = param_1[1] - local_8;
  }
  if (0 < local_c) {
    iVar4 = 0;
    do {
      iVar2 = param_2 % (int)*param_1;
      iVar1 = *param_1 * 0x50 + 0x50;
      iVar3 = DAT_000cef2c + 400 + iVar4;
      param_2 = param_2 + 1;
      iVar4 = iVar4 + 0x50;
      FUN_0001d528(iVar3 + local_10,iVar2 * 0x50 + *(int *)(param_1 + 2) + local_8 * iVar1,iVar1,
                   local_c);
    } while (iVar4 != 400);
  }
  DAT_000cf0e8 = unaff_EBX;
  return;
}

