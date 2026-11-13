
longlong __regparm3 FUN_0003bd40(int param_1,uint param_2)

{
  int *piVar1;
  undefined8 uVar2;
  
  if (((*(int *)(param_1 + 0x5c) != 4) || (*(int *)(param_1 + 0xfe) < *(int *)(param_1 + 4))) &&
     ((*(int *)(param_1 + 0x5c) != 5 || (*(int *)(param_1 + 4) < *(int *)(param_1 + 0xfe))))) {
    if (*(int *)(param_1 + 0x5c) != 3) {
LAB_0003bda2:
      return (ulonglong)param_2 << 0x20;
    }
    uVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
    piVar1 = (int *)((ulonglong)uVar2 >> 0x20);
    if (((int)uVar2 != -1) || (*piVar1 < *(int *)((int)piVar1 + 0xfa))) {
      uVar2 = (**(code **)(piVar1[2] + 0x70))();
      piVar1 = (int *)((ulonglong)uVar2 >> 0x20);
      if (((int)uVar2 != 1) || (*(int *)((int)piVar1 + 0xfa) < *piVar1)) goto LAB_0003bda2;
    }
  }
  return CONCAT44(param_2,1);
}

