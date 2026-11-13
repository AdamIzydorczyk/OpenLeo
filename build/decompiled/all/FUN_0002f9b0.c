
int __regparm3 FUN_0002f9b0(int param_1,int param_2)

{
  undefined8 uVar1;
  
  if (*(int *)(param_1 + 0xf2) != 1) {
    return 1;
  }
  if (*(int *)(param_2 + 0x5c) == 3) {
    uVar1 = (**(code **)(*(int *)(param_2 + 8) + 0x70))();
    if ((int)uVar1 == (int)((ulonglong)uVar1 >> 0x20)) {
      return (int)uVar1;
    }
  }
  return 0;
}

