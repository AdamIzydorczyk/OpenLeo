
void __regparm3 FUN_00052b80(int param_1,undefined4 param_2,undefined4 param_3)

{
  FUN_00052b50(param_2,param_3);
  (**(code **)(*(int *)(param_1 + 8) + 0x14))();
  if (*(int *)(param_1 + 0x28) != 0) {
    (*(code *)**(undefined4 **)(param_1 + 8))();
  }
  return;
}

