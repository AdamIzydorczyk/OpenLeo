
int * __regparm3 FUN_00052dc0(int param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int extraout_ECX;
  int iVar4;
  int extraout_ECX_00;
  int iVar5;
  int iVar6;
  
  piVar1 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x4c))();
  iVar4 = *piVar1;
  iVar6 = piVar1[1];
  iVar2 = FUN_00057500(iVar4,iVar6,piVar1[2],piVar1[3],&PTR_FUN_00098274);
  iVar4 = iVar4 + (*(int *)(extraout_ECX + 0x10) - iVar2);
  iVar2 = FUN_00057530();
  iVar6 = iVar6 + (*(int *)(extraout_ECX_00 + 0x14) - iVar2);
  piVar1 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x4c))();
  iVar5 = iVar4 + (piVar1[2] - *piVar1) + 1;
  iVar3 = (**(code **)(*(int *)(param_1 + 8) + 0x4c))();
  iVar2 = *(int *)(iVar3 + 0xc);
  iVar3 = *(int *)(iVar3 + 4);
  *param_2 = iVar4;
  param_2[1] = iVar6;
  param_2[2] = iVar5;
  param_2[4] = (int)&PTR_FUN_00098274;
  param_2[3] = iVar6 + (iVar2 - iVar3) + 1;
  return param_2;
}

