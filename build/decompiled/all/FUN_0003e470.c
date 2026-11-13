
undefined4 __regparm3 FUN_0003e470(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 extraout_ECX;
  int unaff_EBX;
  
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x6c))();
  iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
  if (iVar1 != 0) {
    *(undefined4 *)(param_1 + 0x11a) = extraout_ECX;
    FUN_000749a0();
    FUN_00049580();
  }
  FUN_000552e0();
  return param_2;
}

