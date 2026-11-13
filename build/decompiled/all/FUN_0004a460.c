
int * __regparm3 FUN_0004a460(int *param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *unaff_EBX;
  
  iVar6 = param_1[1] - (param_1[1] >> 0x1f) >> 1;
  iVar1 = unaff_EBX[1];
  iVar2 = *param_1;
  iVar3 = unaff_EBX[1];
  iVar4 = *unaff_EBX;
  iVar5 = *unaff_EBX;
  param_2[4] = (int)&PTR_FUN_00098274;
  *param_2 = iVar5 - iVar2 / 2;
  param_2[1] = iVar3 - iVar6;
  param_2[2] = iVar4 + iVar2 / 2;
  param_2[3] = iVar1 + iVar6;
  return param_2;
}

