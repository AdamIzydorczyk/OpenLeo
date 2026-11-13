
void FUN_0003b270(void)

{
  undefined4 uVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  undefined1 local_28;
  undefined4 local_27;
  undefined4 local_23;
  undefined **local_1f;
  undefined4 local_1b;
  undefined4 local_17;
  undefined4 local_10;
  
  local_27 = 0;
  local_23 = 0;
  local_1b = 0;
  local_17 = 0;
  local_1f = &PTR_FUN_000961fc;
  FUN_000497b0();
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(extraout_ECX + 0x112) = uVar1;
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(extraout_ECX_00 + 0x12a) = uVar1;
  FUN_00049f90();
  FUN_00049f90();
  FUN_00049920(&local_28,s_damage____d_0009139c,extraout_ECX_01 + 0xb1);
  FUN_00049920(&local_28,s_speed____d_000913aa,&local_10);
  *(undefined4 *)(extraout_ECX_02 + 0xf6) = local_10;
  FUN_00049920(&local_28,s_invincible____d_000913b7,extraout_ECX_02 + 0x15a);
  FUN_00049920(&local_28,s_temp____d_000913c9,extraout_ECX_03 + 0x15e);
  FUN_00049a10();
  FUN_00057a00();
  return;
}

