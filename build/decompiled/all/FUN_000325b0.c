
void __regparm3 FUN_000325b0(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  undefined4 extraout_EDX_02;
  uint local_c;
  undefined4 uStack_8;
  
  uStack_8 = param_3;
  FUN_00049920(param_2,s_x_start____d_00090e80,param_1 + 0x3c);
  FUN_00049920(extraout_EDX,s_x_end____d_00090e8f,param_1 + 0x40);
  FUN_00049920(extraout_EDX_00,s_time_no_move____d_00090e9c,param_1 + 0x44);
  FUN_00049920(extraout_EDX_01,s_time_relax____d_00090eb0,param_1 + 0x48);
  FUN_00049920(extraout_EDX_02,s_time_rand____d_00090ec2,&local_c);
  *(int *)(param_1 + 0x5c) = 0x7fff - (int)(0x7fff / (ulonglong)local_c);
  FUN_00049920(extraout_ECX,s_offset____d_00090ed3,param_1 + 0x58);
  FUN_00049920(extraout_ECX_00,s_direction____d_00090ee1,param_1 + 0x60);
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(param_1 + 100) = uVar1;
  iVar2 = FUN_00076bcc();
  *(undefined4 *)(param_1 + 0x54) = 0;
  *(uint *)(param_1 + 0x50) = (uint)(iVar2 * *(int *)(param_1 + 0x48)) / 0x7fff;
  *(undefined4 *)(param_1 + 0x4c) = *(undefined4 *)(param_1 + 0x44);
  return;
}

