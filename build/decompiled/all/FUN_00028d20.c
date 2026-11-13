
void __regparm3 FUN_00028d20(int param_1)

{
  int iVar1;
  
  iVar1 = (**(code **)(*(int *)(*(int *)(param_1 + 0xee) + 8) + 0xdc))();
  *(undefined4 *)(iVar1 + 0xb1) = *(undefined4 *)(param_1 + 0xb1);
  FUN_00052b10();
  (**(code **)(*(int *)(iVar1 + 8) + 4))();
  *(int *)(iVar1 + 0xee) = param_1;
  (**(code **)(*(int *)(param_1 + 8) + 0x80))();
  return;
}

