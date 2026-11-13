
undefined4 __regparm3 FUN_000773e4(int *param_1,int param_2,uint param_3)

{
  int extraout_ECX;
  int extraout_EDX;
  uint unaff_EBX;
  bool bVar1;
  
  if (0x7fffffff < param_3) {
    return 0xffffffff;
  }
  if (unaff_EBX < 0x200) {
    bVar1 = unaff_EBX == 0x100;
  }
  else {
    if (unaff_EBX < 0x201) goto LAB_00077410;
    bVar1 = unaff_EBX == 0x400;
  }
  if (!bVar1) {
    return 0xffffffff;
  }
LAB_00077410:
  if ((param_2 != 0) && (param_3 == 0)) {
    return 0xffffffff;
  }
  FUN_0007a417();
  if (extraout_ECX != 0) {
    param_1[5] = extraout_ECX;
  }
  param_1[2] = extraout_EDX;
  *(byte *)((int)param_1 + 0xd) = *(byte *)((int)param_1 + 0xd) & 0xf8;
  *param_1 = extraout_EDX;
  param_1[3] = param_1[3] | unaff_EBX;
  if (extraout_EDX == 0) {
    FUN_0007a448();
  }
  return 0;
}

