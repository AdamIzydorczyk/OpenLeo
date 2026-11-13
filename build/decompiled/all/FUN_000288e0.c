
undefined4 __regparm3 FUN_000288e0(int param_1,undefined4 param_2)

{
  int iVar1;
  int *piVar2;
  int unaff_EBX;
  int iVar3;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xd8))();
  if (iVar1 != 0) {
    FUN_00045320();
  }
  piVar2 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x4c))();
  iVar3 = piVar2[1] + 0x10;
  iVar1 = piVar2[3];
  if (piVar2[3] < iVar3) {
    iVar1 = iVar3;
  }
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))
                    (*piVar2 + 5,iVar3,piVar2[2] + -5,iVar1,&PTR_FUN_00098274);
  if (iVar1 != 0) {
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x6c))();
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar1 != 0) {
      *(undefined4 *)(*(int *)(DAT_000cef40 + 0x394) + 0x106) = 0;
    }
  }
  FUN_000552e0();
  return param_2;
}

