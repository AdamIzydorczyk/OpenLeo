
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_00073e70(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 extraout_ECX;
  int local_c;
  undefined4 uStack_8;
  
  uStack_8 = param_3;
  FUN_00040eb0();
  FUN_00049920(extraout_ECX,s_JumpCoef1000____d_00095ae0,&local_c);
  *(float *)(param_1 + 0x120) = (float)local_c / (float)_DAT_00095af3;
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(param_1 + 0x124) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x128) = uVar1;
  return;
}

