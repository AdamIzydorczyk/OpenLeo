
undefined4 __regparm3 FUN_000564a0(int *param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar3;
  int extraout_EDX;
  int *unaff_EBX;
  int iVar4;
  undefined8 uVar5;
  
  if (*param_1 != 0) {
    iVar4 = 0;
    iVar1 = (**(code **)(*(int *)(param_2 + 8) + 8))();
    if (0 < *(int *)(extraout_ECX + 0x24)) {
      iVar3 = 0;
      iVar2 = extraout_ECX;
      do {
        uVar5 = (**(code **)(*(int *)(*(int *)(iVar3 + *(int *)(iVar2 + 0x40)) + 8) + 8))();
        iVar3 = (int)((ulonglong)uVar5 >> 0x20);
        if ((int)uVar5 < iVar1) {
          (**(code **)(*(int *)(*(int *)(iVar3 + *(int *)(extraout_ECX_00 + 0x40)) + 8) + 0x1c))();
          *unaff_EBX = iVar4;
          *(int *)(extraout_EDX + *(int *)(extraout_ECX_01 + 0x40)) = param_2;
          return 1;
        }
        iVar4 = iVar4 + 1;
        iVar3 = iVar3 + 4;
        iVar2 = extraout_ECX_00;
      } while (iVar4 < *(int *)(extraout_ECX_00 + 0x24));
    }
  }
  return 0;
}

