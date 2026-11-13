
void __regparm3 FUN_0003c070(int param_1,undefined4 param_2,undefined4 param_3)

{
  int extraout_EDX;
  int iVar1;
  undefined8 uVar2;
  
  uVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x70))(param_3);
  iVar1 = (int)((ulonglong)uVar2 >> 0x20);
  if ((int)uVar2 == -1) {
    FUN_00053670();
    iVar1 = extraout_EDX;
  }
  *(byte *)(iVar1 + 6) = *(byte *)(iVar1 + 6) | 0x40;
  return;
}

