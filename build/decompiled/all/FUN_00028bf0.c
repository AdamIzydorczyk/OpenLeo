
void __regparm3 FUN_00028bf0(int param_1)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  
  if (*(int *)(param_1 + 0xf6) == 0) {
    if (*(int *)(param_1 + 0xee) != 0) {
      iVar1 = *(int *)(*(int *)(param_1 + 0xee) + 8);
      (**(code **)(*(int *)(param_1 + 8) + 0x70))();
      (**(code **)(iVar1 + 4))();
      (**(code **)(*(int *)(extraout_ECX + 8) + 0x80))();
      param_1 = extraout_ECX_00;
    }
    if (*(int *)(param_1 + 0xf2) != 0) {
      iVar1 = *(int *)(*(int *)(param_1 + 0xf2) + 8);
      (**(code **)(*(int *)(param_1 + 8) + 0x70))();
      (**(code **)(iVar1 + 4))();
      (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x80))();
    }
  }
  return;
}

