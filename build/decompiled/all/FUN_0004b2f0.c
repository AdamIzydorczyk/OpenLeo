
void __regparm3 FUN_0004b2f0(int param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_EDX;
  undefined8 uVar2;
  
  iVar1 = *(int *)(param_1 + 100);
  if (iVar1 == 0) {
    return;
  }
  do {
    uVar2 = (**(code **)(*(int *)(iVar1 + 8) + 0x60))(param_2);
    iVar1 = (int)((ulonglong)uVar2 >> 0x20);
    if ((int)uVar2 != 0) {
      (**(code **)(*(int *)(iVar1 + 8) + 0x74))();
      iVar1 = extraout_EDX;
    }
    iVar1 = *(int *)(iVar1 + 0x34);
  } while (iVar1 != 0);
  return;
}

