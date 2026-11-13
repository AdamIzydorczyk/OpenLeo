
void __regparm3 FUN_00077cd2(int *param_1,int param_2)

{
  int iVar1;
  
  *param_1 = param_2;
  iVar1 = *(int *)(param_2 + 8);
  *(undefined4 *)((int)param_1 + 9) = 0;
  param_1[1] = iVar1;
  (**(code **)(*(int *)(&DAT_000cf518 + **(int **)(param_2 + 4) * 4) + 8))();
  return;
}

