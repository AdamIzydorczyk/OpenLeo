
void __regparm3 FUN_0004ab80(int param_1,undefined4 param_2,undefined4 param_3)

{
  int extraout_EDX;
  
  if (*(int *)(param_1 + 0x3a8) == 0) {
    (**(code **)(*(int *)(*(int *)(param_1 + 0x394) + 8) + 0xec))(param_2,param_3);
    (**(code **)(*(int *)(*(int *)(extraout_EDX + 0x394) + 8) + 0xe8))();
    FUN_00057600();
  }
  return;
}

