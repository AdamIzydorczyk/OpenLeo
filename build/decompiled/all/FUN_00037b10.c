
void __regparm3 FUN_00037b10(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 extraout_ECX;
  int iVar2;
  
  FUN_0003dd60(param_3);
  FUN_00049920(extraout_ECX,s_line____d_000911d1,(int)param_1 + 0x132);
  iVar1 = *(int *)((int)param_1 + 0x132);
  iVar2 = iVar1 + -1;
  *(int *)((int)param_1 + 0x132) = iVar2;
  if (iVar2 < 5) {
    iVar1 = iVar1 * 0x37 + 0x3c5;
    *param_1 = iVar1;
    if (iVar1 < 0x46c) {
      iVar1 = 0x140;
    }
    else {
      iVar1 = 500;
    }
    param_1[1] = iVar1;
    *(int *)((int)param_1 + 0x12a) = *param_1;
    *(int *)((int)param_1 + 0x122) = *param_1;
    iVar1 = param_1[1];
    param_1[1] = 0x28a;
    *(int *)((int)param_1 + 0x12e) = iVar1;
    *(int *)((int)param_1 + 0x126) = iVar1;
  }
  return;
}

