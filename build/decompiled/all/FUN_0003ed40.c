
void FUN_0003ed40(void)

{
  undefined4 uVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  
  uVar1 = FUN_0004dcc0();
  *(undefined4 *)(extraout_ECX + 0x3c) = uVar1;
  (**(code **)(*(int *)(extraout_ECX + 8) + 0x80))();
  *(undefined4 *)(extraout_ECX_00 + 0x40) = 0;
  return;
}

