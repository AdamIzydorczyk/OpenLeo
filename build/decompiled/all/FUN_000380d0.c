
undefined4 __regparm3 FUN_000380d0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int unaff_EBX;
  
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if ((iVar1 != 0) && ((*(int *)(unaff_EBX + 0x5c) == 0x1f || (*(int *)(unaff_EBX + 0x5c) == 0x20)))
     ) {
    FUN_00045320();
  }
  FUN_000553e0();
  return param_2;
}

