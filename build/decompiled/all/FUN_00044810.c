
undefined8 __regparm3 FUN_00044810(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  int extraout_ECX_01;
  int extraout_ECX_02;
  
  if (*(int *)(param_1 + 0x60) == 0) {
    if (*(int *)(param_1 + 100) != 0) {
      (**(code **)(*(int *)(**(int **)(*(int *)(param_1 + 0x88) + 8) + 0x33) + 0x10))();
      param_1 = (**(code **)(*(int *)(*(int *)(*(int *)(*(int *)(extraout_ECX_01 + 0x88) + 8) + 4) +
                                     0x33) + 0x10))();
      *(int *)(extraout_ECX_02 + 100) = *(int *)(extraout_ECX_02 + 100) + -1;
    }
    return CONCAT44(param_2,param_1);
  }
  (**(code **)(*(int *)(**(int **)(*(int *)(param_1 + 0x88) + 8) + 0x33) + 0x10))();
  iVar1 = *(int *)(extraout_ECX + 0x60) / 3;
  iVar2 = iVar1 / 2;
  iVar3 = extraout_ECX;
  if (iVar1 % 2 != 0) {
    iVar2 = (**(code **)(*(int *)(*(int *)(*(int *)(*(int *)(extraout_ECX + 0x88) + 8) + 4) + 0x33)
                        + 0x10))();
    iVar3 = extraout_ECX_00;
  }
  *(int *)(iVar3 + 0x60) = *(int *)(iVar3 + 0x60) + -1;
  return CONCAT44(param_2,iVar2);
}

