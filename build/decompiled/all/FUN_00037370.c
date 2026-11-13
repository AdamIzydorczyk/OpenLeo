
int __regparm3 FUN_00037370(undefined4 param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  int *extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int unaff_EBX;
  int iVar4;
  
  iVar2 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar2 != 0) {
    iVar4 = *extraout_ECX + 10;
    iVar2 = extraout_ECX[1] + 10;
    piVar3 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))(iVar4,iVar2,&PTR_LAB_00098278);
    if ((((iVar4 < *piVar3) || (piVar3[2] < iVar4)) || (iVar2 < piVar3[1])) || (piVar3[3] < iVar2))
    {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (bVar1) {
      iVar2 = extraout_ECX_00;
      if (*(int *)(extraout_ECX_00 + 0x44) != -1) {
        FUN_00052cc0();
        iVar2 = extraout_ECX_01;
      }
      (**(code **)(*(int *)(iVar2 + 8) + 0x84))();
      FUN_00052b10();
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

