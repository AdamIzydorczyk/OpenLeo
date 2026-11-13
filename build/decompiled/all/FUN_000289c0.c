
undefined4 __regparm3 FUN_000289c0(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  bool bVar8;
  int *piVar9;
  
  piVar9 = (int *)(**(code **)(*(int *)(param_2 + 8) + 0x6c))();
  iVar1 = *piVar9;
  iVar2 = piVar9[1];
  piVar9 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x4c))();
  iVar3 = *piVar9;
  iVar4 = piVar9[1];
  iVar5 = piVar9[2];
  iVar6 = piVar9[3];
  FUN_0004a050();
  iVar7 = *(int *)(*(int *)(*(int *)(param_1 + 0x5c) * 4 + *(int *)(param_1 + 0x50)) + 4);
  if ((1 < iVar7) && (iVar7 < 0xc)) {
    if ((iVar1 < iVar3) || (((iVar5 < iVar1 || (iVar2 + 8 < iVar4)) || (iVar6 < iVar2 + 8)))) {
      bVar8 = false;
    }
    else {
      bVar8 = true;
    }
    if (bVar8) {
      return 0;
    }
  }
  return 1;
}

