
void __regparm3 FUN_000541f0(undefined4 *param_1,undefined4 *param_2)

{
  int iVar1;
  
  *param_2 = *param_1;
  param_2[1] = param_1[1];
  if ((*(byte *)((int)param_1 + 0xb6) & 2) == 0) {
    iVar1 = (**(code **)(param_1[2] + 0xbc))();
    if (iVar1 != 0) {
      return;
    }
    if ((*(byte *)((int)param_1 + 0xb6) & 1) != 0) {
      return;
    }
  }
  iVar1 = (**(code **)(param_1[2] + 0x48))();
  iVar1 = *(int *)(iVar1 + 0x33);
  (**(code **)(param_1[2] + 0x50))();
  (**(code **)(iVar1 + 4))();
  return;
}

