
undefined4 __regparm3 FUN_000550c0(undefined4 param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  uint uVar4;
  
  iVar1 = (**(code **)(*(int *)(param_2 + 8) + 0x40))();
  if (iVar1 != 0) {
    iVar1 = (**(code **)(*(int *)(param_2 + 8) + 0x70))();
    piVar2 = (int *)FUN_00052d30();
    piVar3 = (int *)FUN_00052d30();
    if (*piVar2 - *piVar3 < 0) {
      uVar4 = 0xffffffff;
    }
    else {
      uVar4 = (uint)(0 < *piVar2 - *piVar3);
    }
    if ((int)(uVar4 * iVar1) < 0) {
      return 1;
    }
  }
  return 0;
}

