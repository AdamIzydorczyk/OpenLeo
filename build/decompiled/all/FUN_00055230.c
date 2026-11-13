
void __regparm3 FUN_00055230(int param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x44))();
  if ((uVar1 & 0x80000) != 0) {
    (**(code **)(*(int *)(param_2 + 8) + 0xd0))();
  }
  iVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x40))();
  if (iVar2 == 0) {
    uVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x44))();
    if ((uVar1 & 0x2000) != 0) {
      (**(code **)(*(int *)(param_2 + 8) + 0xd0))();
    }
  }
  else {
    FUN_00055130();
    FUN_00049580();
  }
  FUN_000749a0();
  FUN_00049580();
  return;
}

