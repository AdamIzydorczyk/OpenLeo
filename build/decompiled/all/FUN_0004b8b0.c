
undefined4 __regparm3 FUN_0004b8b0(undefined4 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined4 uVar4;
  
  iVar1 = FUN_00057500();
  iVar2 = FUN_00057530();
  piVar3 = (int *)(**(code **)(*(int *)(param_2 + 8) + 0x4c))();
  if ((((piVar3[3] < iVar2) || (iVar2 + 200 < piVar3[1])) || (piVar3[2] < iVar1)) ||
     (iVar1 + 0x140 < *piVar3)) {
    uVar4 = 0;
  }
  else {
    uVar4 = 1;
  }
  return uVar4;
}

