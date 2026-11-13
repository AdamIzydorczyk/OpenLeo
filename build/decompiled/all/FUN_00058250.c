
void __regparm3
FUN_00058250(int *param_1,int param_2,int param_3,int param_4,int param_5,int param_6)

{
  uint uVar1;
  int unaff_EBX;
  
  uVar1 = param_2 - param_3 >> 0x1f;
  *param_1 = param_2 + (int)((param_2 - param_3 ^ uVar1) - uVar1) / 2;
  uVar1 = param_5 - param_3 >> 0x1f;
  param_1[1] = param_5 - (int)((param_5 - param_3 ^ uVar1) - uVar1) / 2;
  uVar1 = unaff_EBX - param_4 >> 0x1f;
  param_1[2] = unaff_EBX + (int)((unaff_EBX - param_4 ^ uVar1) - uVar1) / 2;
  uVar1 = param_6 - param_4 >> 0x1f;
  param_1[3] = param_6 - (int)((param_6 - param_4 ^ uVar1) - uVar1) / 2;
  return;
}

