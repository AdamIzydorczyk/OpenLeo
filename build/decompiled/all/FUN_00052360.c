
void __regparm3 FUN_00052360(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int extraout_EDX;
  int iVar3;
  undefined8 uVar4;
  
  FUN_00052070(param_3);
  *(undefined4 *)(*param_1 + 0xa1) = 0;
  *(undefined4 *)(*param_1 + 0x99) = 1;
  if (extraout_EDX < 0) {
    return;
  }
  iVar3 = extraout_EDX;
  if ((*(byte *)((int)param_1 + 0x12) & 0x10) != 0) {
    uVar4 = (**(code **)(*(int *)(*param_1 + 8) + 0x70))();
    iVar3 = (int)((ulonglong)uVar4 >> 0x20);
    if ((int)uVar4 < 0) {
      iVar2 = 1;
      goto LAB_000523a0;
    }
  }
  iVar2 = 0;
LAB_000523a0:
  if (((*(byte *)((int)param_1 + 0x12) & 0x20) == 0) || (-1 < *(int *)(*param_1 + 0x71))) {
    iVar1 = 0;
  }
  else {
    iVar1 = 1;
  }
  if (iVar1 == iVar2) {
    param_1[1] = iVar3;
    return;
  }
  param_1[1] = (*(int *)((int)param_1 + 0x36) - iVar3) + -1;
  return;
}

