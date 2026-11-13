
void __regparm3 FUN_0002e590(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_0005c320(param_2,param_3);
  iVar2 = 0;
  if (iVar1 != 0) {
    iVar2 = FUN_00056620();
  }
  *(int *)(param_1 + 0x12a) = iVar2;
  (**(code **)(*(int *)(iVar2 + 8) + 0x24))();
  (**(code **)(*(int *)(*(int *)(param_1 + 0x12a) + 8) + 0x28))();
  return;
}

