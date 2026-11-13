
void __regparm3 FUN_00027d60(int param_1)

{
  int *piVar1;
  int iVar2;
  longdouble lVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  
  uVar6 = 0;
  piVar1 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
  iVar4 = *(int *)(param_1 + 0x126);
  iVar5 = *piVar1;
  iVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x68))();
  if ((iVar4 - iVar5 < 1) && (-1 < *(int *)(param_1 + 0x12a) - *(int *)(iVar2 + 4))) {
    iVar4 = 0;
    iVar5 = 0;
  }
  else {
    lVar3 = (longdouble)FUN_00075d96();
    iVar4 = (int)ROUND(lVar3);
    lVar3 = (longdouble)FUN_00075d96(iVar4);
    iVar5 = (int)ROUND(lVar3);
  }
  (**(code **)(*(int *)(param_1 + 8) + 0xcc))(iVar4,iVar5,uVar6);
  FUN_000550a0();
  return;
}

