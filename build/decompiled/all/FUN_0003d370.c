
int __regparm3 FUN_0003d370(undefined4 param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  int extraout_ECX;
  undefined4 extraout_ECX_00;
  int unaff_EBX;
  int iVar4;
  undefined **ppuVar5;
  
  iVar2 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if ((iVar2 != 0) && (*(int *)(extraout_ECX + 0x5c) == 1)) {
    piVar3 = (int *)(**(code **)(*(int *)(extraout_ECX + 8) + 0x68))();
    ppuVar5 = &PTR_LAB_00098278;
    iVar4 = *piVar3 + 10;
    iVar2 = piVar3[1] + 10;
    piVar3 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
    if ((iVar4 < *piVar3) || (((piVar3[2] < iVar4 || (iVar2 < piVar3[1])) || (piVar3[3] < iVar2))))
    {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (bVar1) {
      (**(code **)(*(int *)(unaff_EBX + 8) + 0x10))(0,9,extraout_ECX_00,0,iVar4,iVar2,ppuVar5);
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

