
byte * __regparm3 FUN_0001ef80(int param_1,byte *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  bool bVar4;
  int *piVar5;
  int iVar6;
  int unaff_EBX;
  
  piVar5 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
  iVar6 = *piVar5;
  iVar1 = piVar5[1];
  iVar2 = piVar5[2];
  iVar3 = piVar5[3];
  FUN_0004a050();
  piVar5 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x4c))();
  if (2 < *(int *)(param_1 + 0xee)) {
    if ((((iVar3 < piVar5[1]) || (piVar5[3] < iVar1)) || (iVar2 < *piVar5)) || (piVar5[2] < iVar6))
    {
      bVar4 = false;
    }
    else {
      bVar4 = true;
    }
    if (bVar4) {
      iVar6 = (**(code **)(*(int *)(param_1 + 8) + 0x40))();
      if (iVar6 == 0) {
        if ((*(uint *)(*(int *)(*(int *)(param_1 + 0x50) + *(int *)(param_1 + 0x5c) * 4) + 0x12) &
            0x2000) != 0) {
          (**(code **)(*(int *)(unaff_EBX + 8) + 0xd0))();
        }
        param_2[0xc] = 0;
        FUN_000749a0();
        *param_2 = *param_2 | 0x80;
        return param_2;
      }
      iVar6 = FUN_000550c0();
      if (iVar6 == 0) {
        (**(code **)(*(int *)(unaff_EBX + 8) + 0xd0))();
      }
      param_2[0xc] = 0;
      FUN_000749a0();
      param_2[2] = param_2[2] | 1;
      return param_2;
    }
  }
  param_2[0xc] = 0;
  FUN_000749a0();
  return param_2;
}

