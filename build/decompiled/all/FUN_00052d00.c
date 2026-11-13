
void __regparm3 FUN_00052d00(int param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_EDX;
  undefined8 uVar2;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x30))(param_2);
  uVar2 = (**(code **)(*(int *)(iVar1 + 8) + 0x34))();
  if ((int)uVar2 != 0) {
    iVar1 = (**(code **)(*(int *)((int)((ulonglong)uVar2 >> 0x20) + 8) + 0x30))();
    (**(code **)(*(int *)(iVar1 + 8) + 0x30))();
    *(undefined4 *)(extraout_EDX + 0x20) = 0;
  }
  return;
}

