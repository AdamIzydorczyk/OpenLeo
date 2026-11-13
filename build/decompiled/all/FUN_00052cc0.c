
void __regparm3 FUN_00052cc0(int param_1)

{
  int iVar1;
  int extraout_ECX;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x30))();
  (**(code **)(*(int *)(iVar1 + 8) + 0x24))();
  (**(code **)(*(int *)(iVar1 + 8) + 0x28))();
  FUN_00056600();
  *(undefined4 *)(extraout_ECX + 0x20) = 1;
  return;
}

