
undefined4 __regparm3 FUN_00027f30(int param_1,undefined4 param_2)

{
  int iVar1;
  int unaff_EBX;
  
  if (*(int *)(param_1 + 0x5c) == 1) {
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
    if ((iVar1 != 0) && (*(int *)(unaff_EBX + 0x5c) == 0x10)) {
      FUN_000749a0();
      FUN_00049580();
    }
  }
  FUN_0003df00();
  return param_2;
}

