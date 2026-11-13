
int __regparm3 FUN_00028af0(int param_1,int param_2)

{
  int iVar1;
  bool bVar2;
  int iVar3;
  int *piVar4;
  int unaff_EBX;
  
  if (*(int *)(param_1 + 0x5c) == 4) {
    if (*(int *)(param_1 + 0xf6) == 0) {
      FUN_00028bf0();
      *(undefined4 *)(param_1 + 0xf6) = 1;
    }
    iVar3 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
    if (iVar3 != 0) {
      piVar4 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
      iVar3 = *piVar4;
      iVar1 = piVar4[1];
      piVar4 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
      if ((((iVar3 < *piVar4) || (piVar4[2] < iVar3)) || (iVar1 + -0x14 < piVar4[1])) ||
         (piVar4[3] < iVar1 + -0x14)) {
        bVar2 = false;
      }
      else {
        bVar2 = true;
      }
      if (bVar2) {
        FUN_00052b10();
        FUN_000749a0();
        FUN_00049580();
      }
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

