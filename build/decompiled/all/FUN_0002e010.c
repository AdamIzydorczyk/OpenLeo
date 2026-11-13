
void FUN_0002e010(void)

{
  undefined4 uVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  undefined1 local_24;
  undefined4 local_23;
  undefined4 local_1f;
  undefined **local_1b;
  undefined4 local_17;
  undefined4 local_13;
  
  local_23 = 0;
  local_1f = 0;
  local_17 = 0;
  local_13 = 0;
  local_1b = &PTR_FUN_000961fc;
  FUN_000497b0();
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(extraout_ECX + 0x112) = uVar1;
  FUN_00049920(&local_24,s_damage____d_00090bad,extraout_ECX + 0xb1);
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(extraout_ECX_00 + 0x13a) = uVar1;
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(extraout_ECX_01 + 0x13e) = uVar1;
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(extraout_ECX_02 + 0x142) = uVar1;
  FUN_0002de30();
  FUN_00049f90();
  FUN_00049920(&local_24,s_invincible____d_00090bbb,extraout_ECX_03 + 0x1ce);
  *(undefined4 *)(extraout_ECX_04 + 0x1ca) = 0xffffffff;
  FUN_00049a10();
  FUN_00057a00();
  return;
}

