
int __regparm3 FUN_00028590(int *param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  int unaff_EBX;
  int iVar4;
  
  if ((param_1[0x17] == 4) && (*(int *)((int)param_1 + 0x112) == 0)) {
    if ((*(int *)(*(int *)(DAT_000cef40 + 0x394) + 0x5c) == 4) &&
       (0x3b < *(int *)(*(int *)(*(int *)(*(int *)(DAT_000cef40 + 0x394) + 0x50) + 0x10) + 4))) {
      FUN_000283e0();
      *(undefined4 *)((int)param_1 + 0x112) = 1;
    }
  }
  if (param_1[0x17] != 3) {
    FUN_0003d440();
    return param_2;
  }
  iVar2 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar2 != 0) {
    iVar4 = *param_1;
    iVar2 = param_1[1] + -0x14;
    piVar3 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))(iVar4,iVar2,&PTR_LAB_00098278);
    if ((((iVar4 < *piVar3) || (piVar3[2] < iVar4)) || (iVar2 < piVar3[1])) || (piVar3[3] < iVar2))
    {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (bVar1) {
      FUN_00052b10();
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

