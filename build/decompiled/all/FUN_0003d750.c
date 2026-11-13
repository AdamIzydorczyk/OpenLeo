
undefined4 __regparm3 FUN_0003d750(int param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int unaff_EBX;
  
  puVar1 = (undefined4 *)(**(code **)(*(int *)(param_1 + 8) + 0x4c))();
  FUN_0004a050(*puVar1,puVar1[1],puVar1[2],puVar1[3],&PTR_FUN_00098274);
  FUN_0004a080();
  iVar2 = (**(code **)(*(int *)(extraout_ECX + 8) + 0xe0))();
  if (((*(int *)(param_2 + 0x5c) == iVar2) &&
      (iVar2 = (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0xec))(),
      iVar2 < *(int *)(*(int *)(*(int *)(param_2 + 0x5c) * 4 + *(int *)(param_2 + 0x50)) + 4))) &&
     (iVar2 = (**(code **)(*(int *)(unaff_EBX + 8) + 8))(), iVar2 != 0)) {
    return 1;
  }
  return 0;
}

