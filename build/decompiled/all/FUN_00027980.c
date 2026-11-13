
int __regparm3 FUN_00027980(int param_1)

{
  bool bVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  longdouble extraout_ST0;
  longdouble extraout_ST0_00;
  undefined8 uVar4;
  double dVar5;
  
  dVar5 = (double)*(int *)(param_1 + 0x1a2);
  uVar4 = FUN_00075d96();
  iVar2 = (int)uVar4;
  if ((*(int *)(iVar2 + 0x13e) + -2 < (int)ROUND(extraout_ST0)) ||
     ((int)ROUND(extraout_ST0) < *(int *)(iVar2 + 0x146) + 2)) {
    uVar4 = FUN_00075d96((double)*(int *)(iVar2 + 0x1a2),dVar5);
    if ((*(int *)((int)uVar4 + 0x146) + -2 < (int)ROUND(extraout_ST0_00)) ||
       ((int)ROUND(extraout_ST0_00) < *(int *)((int)uVar4 + 0x13e) + 2)) {
      bVar1 = false;
      goto LAB_00027a03;
    }
  }
  bVar1 = true;
LAB_00027a03:
  iVar2 = (int)uVar4;
  if ((bVar1) && (*(int *)(iVar2 + 0x19a) == 0)) {
    iVar3 = 1;
  }
  else {
    iVar3 = 0;
  }
  if ((iVar3 == 0) && ((int)((ulonglong)uVar4 >> 0x20) == *(int *)(iVar2 + 0x112))) {
    (**(code **)(*(int *)(iVar2 + 8) + 0xe0))();
    iVar3 = extraout_ECX;
  }
  return iVar3;
}

