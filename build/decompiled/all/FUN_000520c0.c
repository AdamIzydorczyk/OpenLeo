
void __regparm3 FUN_000520c0(int *param_1,byte *param_2)

{
  byte bVar1;
  int iVar2;
  byte bVar3;
  undefined4 unaff_EBX;
  
  iVar2 = *(int *)(*(int *)((int)param_1 + 0x2e) + param_1[1] * 4);
  (**(code **)(*(int *)(iVar2 + 4) + 4))();
  (*(code *)PTR_FUN_0009827c)();
  (**(code **)(param_1[0x11] + 0x28))();
  (**(code **)(*(int *)(*param_1 + 8) + 4))();
  *(undefined4 *)(*param_1 + 0xa1) = unaff_EBX;
  (**(code **)(*(int *)(iVar2 + 4) + 0x10))();
  iVar2 = (**(code **)(*(int *)(*param_1 + 8) + 0x70))();
  if ((iVar2 == -1) && ((*(byte *)((int)param_1 + 0x12) & 0x10) != 0)) {
    bVar1 = 1;
  }
  else {
    bVar1 = 0;
  }
  if ((*(int *)(*param_1 + 0x71) == -1) && ((*(byte *)((int)param_1 + 0x12) & 0x20) != 0)) {
    bVar3 = 1;
  }
  else {
    bVar3 = 0;
  }
  if ((bool)(bVar3 ^ bVar1) == (*(int *)(*param_1 + 0x99) == -1)) {
    if (*(int *)((int)param_1 + 0x36) + -1 == param_1[1]) {
      *param_2 = *param_2 | 4;
      param_1[1] = 0;
      return;
    }
    param_1[1] = param_1[1] + 1;
  }
  else if (param_1[1] == 0) {
    *param_2 = *param_2 | 8;
    param_1[1] = *(int *)((int)param_1 + 0x36) + -1;
  }
  else {
    param_1[1] = param_1[1] + -1;
  }
  return;
}

