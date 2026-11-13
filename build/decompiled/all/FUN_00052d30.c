
int * __regparm3 FUN_00052d30(int param_1,int *param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar5;
  int iVar6;
  
  piVar2 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
  iVar5 = *piVar2;
  iVar6 = piVar2[1];
  iVar3 = FUN_00057530(iVar5,iVar6,&PTR_LAB_00098278);
  iVar1 = *(int *)(extraout_ECX + 0x14);
  iVar4 = FUN_00057500();
  *param_2 = iVar5 + (*(int *)(extraout_ECX_00 + 0x10) - iVar4);
  param_2[2] = (int)&PTR_LAB_00098278;
  param_2[1] = iVar6 + (iVar1 - iVar3);
  return param_2;
}

