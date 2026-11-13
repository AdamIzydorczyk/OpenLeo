
undefined4 __regparm3 FUN_0001f210(int param_1,undefined4 param_2)

{
  bool bVar1;
  int iVar2;
  int *piVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int *piVar4;
  int unaff_EBX;
  undefined8 uVar5;
  
  if (*(int *)(param_1 + 0x5c) == 1) {
    iVar2 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
    if (iVar2 != 0) {
      (**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
      uVar5 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x4c))();
      piVar4 = (int *)((ulonglong)uVar5 >> 0x20);
      piVar3 = (int *)uVar5;
      if ((((piVar4[3] < piVar3[1]) || (piVar3[3] < piVar4[1])) || (piVar4[2] < *piVar3)) ||
         (piVar3[2] < *piVar4)) {
        bVar1 = false;
      }
      else {
        bVar1 = true;
      }
      if ((bVar1) && (*(int *)(unaff_EBX + 0x5c) == 9)) {
        *(byte *)(extraout_ECX_00 + 0x66) = *(byte *)(extraout_ECX_00 + 0x66) | 0x80;
      }
    }
  }
  FUN_0003df00();
  return param_2;
}

