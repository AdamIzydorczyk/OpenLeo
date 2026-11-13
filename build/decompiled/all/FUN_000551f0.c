
void __regparm3 FUN_000551f0(int param_1,undefined4 param_2)

{
  int extraout_EDX;
  undefined8 uVar1;
  
  *(undefined4 *)(param_1 + 0xd2) = **(undefined4 **)(param_1 + 0xd2);
  FUN_0005c370(param_2);
  if (*(int *)(extraout_EDX + 0xd2) == 0) {
    uVar1 = (**(code **)(*(int *)(extraout_EDX + 8) + 0x3c))();
    if ((int)uVar1 != 0) {
      *(undefined4 *)((int)((ulonglong)uVar1 >> 0x20) + 0x106) = 1;
    }
  }
  return;
}

