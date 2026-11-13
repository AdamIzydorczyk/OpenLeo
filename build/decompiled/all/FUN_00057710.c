
void __regparm3 FUN_00057710(int param_1)

{
  int iVar1;
  int extraout_ECX;
  
  FUN_00057500();
  FUN_00057500();
  iVar1 = *(int *)(*(int *)(*(int *)(param_1 + 0x44) + extraout_ECX * 4) + 8);
  FUN_00057530();
  (**(code **)(iVar1 + 8))();
  return;
}

