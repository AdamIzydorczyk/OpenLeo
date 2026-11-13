
void __regparm3 FUN_000529d0(int param_1,undefined4 param_2,undefined4 param_3)

{
  int extraout_ECX;
  undefined4 *extraout_ECX_00;
  
  (**(code **)(*(int *)(param_1 + 8) + 0x1c))(param_3);
  (**(code **)(*(int *)(extraout_ECX + 8) + 0x18))();
  extraout_ECX_00[5] = *extraout_ECX_00;
  extraout_ECX_00[6] = extraout_ECX_00[1];
  (**(code **)(extraout_ECX_00[2] + 0x54))();
  return;
}

