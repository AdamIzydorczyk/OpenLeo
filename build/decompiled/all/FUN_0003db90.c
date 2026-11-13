
int __regparm3 FUN_0003db90(undefined4 param_1,int param_2)

{
  short sVar1;
  bool bVar2;
  int iVar3;
  int *piVar4;
  int *extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int unaff_EBX;
  int iVar5;
  
  iVar3 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar3 != 0) {
    iVar3 = *extraout_ECX;
    iVar5 = extraout_ECX[1];
    piVar4 = (int *)FUN_000229c0(iVar3,iVar5,&PTR_LAB_00098278);
    sVar1 = *(short *)(*piVar4 + 0x17);
    piVar4 = (int *)FUN_000229c0();
    iVar5 = iVar5 + (int)sVar1 / 2;
    iVar3 = iVar3 + ((int)*(short *)(*piVar4 + 0x15) + *(short *)(*piVar4 + 0x19) * 0x50) / 2;
    piVar4 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
    if ((((iVar3 < *piVar4) || (piVar4[2] < iVar3)) || (iVar5 < piVar4[1])) || (piVar4[3] < iVar5))
    {
      bVar2 = false;
    }
    else {
      bVar2 = true;
    }
    if (bVar2) {
      iVar3 = extraout_ECX_00;
      if (*(int *)(extraout_ECX_00 + 0x44) != -1) {
        FUN_00052cc0();
        iVar3 = extraout_ECX_01;
      }
      (**(code **)(*(int *)(iVar3 + 8) + 0x84))();
      FUN_00052b10();
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

