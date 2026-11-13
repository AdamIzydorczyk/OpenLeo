
void __regparm3
FUN_00056ef2(short *param_1,int param_2,int param_3,undefined4 param_4,undefined4 param_5,
            short *param_6,undefined4 param_7,int param_8,int param_9)

{
  short sVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  int unaff_EBX;
  int local_20;
  int local_1c;
  int local_18;
  int local_c;
  int local_8;
  
  iVar2 = (**(code **)(*(int *)(param_6 + 4) + 0x10))();
  if (iVar2 == 0) {
    FUN_00056ad0(param_6,extraout_ECX,param_8,param_9);
  }
  else {
    local_1c = 200;
    local_20 = 0;
    local_18 = param_3;
    if (param_3 < 0) {
      local_1c = param_3 + 200;
      local_20 = param_3 * -400;
      local_18 = 0;
    }
    if ((int)param_1[1] < local_18 + local_1c) {
      local_1c = param_1[1] - local_18;
    }
    if (0 < local_1c) {
      sVar1 = param_1[6];
      local_8 = 0;
      iVar2 = extraout_ECX;
      do {
        if (param_8 - unaff_EBX < 1) {
          local_c = (iVar2 + -1) % (int)*param_6 + 1;
        }
        else {
          local_c = iVar2 % (int)*param_6;
        }
        iVar3 = *param_6 * 0x50 + 0x50;
        iVar2 = iVar2 + 1;
        FUN_0001d014(DAT_000cef2c + 400 + local_8 + local_20,
                     local_c * 0x50 + *(int *)(param_6 + 2) + iVar3 * param_9 +
                     (param_8 - unaff_EBX),iVar3,
                     *(int *)(param_1 + 2) +
                     *(int *)(*(int *)(param_1 + 9) + (param_2 % (int)*param_1) * 4) +
                     *(int *)((local_18 / (int)sVar1) * 4 +
                             *(int *)(*(int *)(param_1 + 7) + (param_2 % (int)*param_1) * 4)),
                     local_18 % (int)param_1[6],local_1c);
        param_2 = param_2 + 1;
        local_8 = local_8 + 0x50;
      } while (local_8 != 400);
    }
    DAT_000cf0e8 = unaff_EBX;
  }
  return;
}

