
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00036280(void)

{
  float fVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  undefined1 local_34;
  undefined4 local_33;
  undefined4 local_2f;
  undefined **local_2b;
  undefined4 local_27;
  undefined4 local_23;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  local_33 = 0;
  local_2f = 0;
  local_27 = 0;
  local_23 = 0;
  local_2b = &PTR_FUN_000961fc;
  FUN_000497b0();
  iVar2 = FUN_0004dcc0();
  *(int *)(extraout_ECX + 0x112) = iVar2;
  (**(code **)(*(int *)(iVar2 + 8) + 0x5c))();
  *(undefined4 *)(*(int *)(extraout_ECX_00 + 0x112) + 0xb1) = 0x14;
  iVar2 = FUN_0004dcc0();
  *(int *)(extraout_ECX_01 + 0x12a) = iVar2;
  (**(code **)(*(int *)(iVar2 + 8) + 0x5c))();
  *(undefined4 *)(*(int *)(extraout_ECX_02 + 0x12a) + 0xb1) = 1000000;
  *(undefined4 *)(*(int *)(extraout_ECX_02 + 0x12a) + 0xb1) = 0x29;
  FUN_00049920(&local_34,s_narty_param__d__d__d__d_0009113f,&local_1c,&local_18,&local_14,&local_10)
  ;
  fVar1 = (float)_DAT_00091158;
  *(float *)(extraout_ECX_03 + 0x12e) = (float)local_1c / fVar1;
  *(float *)(extraout_ECX_03 + 0x132) = (float)local_18 / fVar1;
  *(float *)(extraout_ECX_03 + 0x136) = (float)local_14 / fVar1;
  *(float *)(extraout_ECX_03 + 0x13a) = (float)local_10 / fVar1;
  FUN_00049a10();
  FUN_00057a00();
  return;
}

